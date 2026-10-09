"""Negative and positive tests for the repository integrity tools.

Temporary fixtures deliberately contain broken inputs: a checker that always passes
would make the preservation and reproducibility promises meaningless.
"""
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tempfile
import unittest
import zlib

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
from check_source_unchanged import check_sources, cpp_tokens
from verify_assets import check_assets, png_dimensions, require_file


def make_png(width=2, height=2):
    """Construct a tiny fixture with real CRCs, independent of external image libraries."""
    def chunk(kind, content):
        return struct.pack(">I", len(content)) + kind + content + struct.pack(">I", zlib.crc32(kind + content) & 0xFFFFFFFF)
    header = struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)
    pixels = zlib.compress((b"\x00" + b"\x00" * (width * 4)) * height)
    return b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", header) + chunk(b"IDAT", pixels) + chunk(b"IEND", b"")


class AssetValidationTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)

    def fixture(self):
        for folder in ("src/app/game2D", "res/textures", "res/shaders", "res/meshes"):
            (self.root / folder).mkdir(parents=True, exist_ok=True)
        (self.root / "src/app/path.h").write_text(
            'inline const std::string image = std::string(TEXTURE_DIR) + "image.png";\n'
            'inline const std::string model = std::string(MESHES_DIR) + "model.obj";\n'
            'inline const std::string shader = std::string(SHADERS_DIR) + "shader.vert";\n'
        )
        (self.root / "src/app/game2D/game_data.h").write_text(
            "playerSheetRows = 2; playerSheetColumns = 2; catSheetRows = 2; catSheetColumns = 2;"
        )
        for name in ("image.png", "player_spritesheet.png", "cat_spritesheet.png"):
            (self.root / "res/textures" / name).write_bytes(make_png())
        (self.root / "res/shaders/shader.vert").write_text("#version 330 core\nvoid main() {}\n")
        (self.root / "res/meshes/model.obj").write_text("mtllib model.mtl\nvn 0 0 1\nusemtl surface\nf 1 2 3\n")
        (self.root / "res/meshes/model.mtl").write_text("newmtl surface\n")

    def test_png_dimensions_accept_valid_chunks(self):
        """Given an intact image, size extraction must match the image header."""
        path = self.root / "valid.png"
        path.write_bytes(make_png(6, 4))
        self.assertEqual(png_dimensions(path), (6, 4))

    def test_png_rejects_corrupt_checksum(self):
        """A damaged byte must not be accepted as a complete downloaded texture."""
        path = self.root / "corrupt.png"
        data = bytearray(make_png())
        data[20] ^= 1
        path.write_bytes(data)
        with self.assertRaisesRegex(ValueError, "checksum"):
            png_dimensions(path)

    def test_png_rejects_truncated_payload(self):
        """Interrupted downloads must fail before the application opens a window."""
        path = self.root / "truncated.png"
        path.write_bytes(make_png()[:-6])
        with self.assertRaisesRegex(ValueError, "truncated"):
            png_dimensions(path)

    def test_png_rejects_wrong_signature(self):
        """Renaming a text file to PNG must not satisfy the asset contract."""
        path = self.root / "wrong.png"
        path.write_bytes(b"not a PNG")
        with self.assertRaisesRegex(ValueError, "signature"):
            png_dimensions(path)

    def test_file_reference_rejects_missing_asset(self):
        """An absent resource must produce a clear failure instead of a silent pass."""
        with self.assertRaisesRegex(ValueError, "missing"):
            require_file(self.root / "absent", self.root)

    def test_file_reference_rejects_escape_from_checkout(self):
        """A machine-specific external path cannot be part of a self-contained checkout."""
        with self.assertRaisesRegex(ValueError, "escapes"):
            require_file(self.root / ".." / "outside", self.root)

    def test_complete_asset_contract_passes(self):
        """The integration validator must accept compatible shader, image and mesh data."""
        self.fixture()
        self.assertEqual(check_assets(self.root), (3, 1))

    def test_asset_contract_rejects_undefined_material(self):
        """An OBJ material name must resolve in its referenced MTL library."""
        self.fixture()
        (self.root / "res/meshes/model.mtl").write_text("newmtl other\n")
        with self.assertRaisesRegex(ValueError, "undefined OBJ materials"):
            check_assets(self.root)

    def test_asset_contract_rejects_incompatible_sheet_grid(self):
        """Sprite cells must divide the actual image dimensions without partial pixels."""
        self.fixture()
        (self.root / "res/textures/player_spritesheet.png").write_bytes(make_png(3, 2))
        with self.assertRaisesRegex(ValueError, "cannot be split"):
            check_assets(self.root)

    def test_asset_contract_rejects_wrong_shader_version(self):
        """A shader targeting a different OpenGL version must fail this data check."""
        self.fixture()
        (self.root / "res/shaders/shader.vert").write_text("#version 450 core\n")
        with self.assertRaisesRegex(ValueError, "GLSL 330"):
            check_assets(self.root)

    def test_commented_path_is_not_a_live_asset(self):
        """A useful example comment must not invent a required missing resource."""
        self.fixture()
        path = self.root / "src/app/path.h"
        with path.open("a") as stream:
            stream.write('// inline const std::string demo = std::string(TEXTURE_DIR) + "missing.png";\n')
        self.assertEqual(check_assets(self.root), (3, 1))

    def test_commented_version_does_not_mask_active_shader_version(self):
        """GLSL comments must not let an incompatible active version pass."""
        self.fixture()
        (self.root / "res/shaders/shader.vert").write_text("/*\n#version 330 core\n*/\n#version 450 core\n")
        with self.assertRaisesRegex(ValueError, "GLSL 330"):
            check_assets(self.root)


class SourceLexerTests(unittest.TestCase):
    def test_comments_do_not_change_tokens(self):
        """Useful added comments must be allowed by the production preservation guard."""
        self.assertEqual(cpp_tokens(b"int x = 1;"), cpp_tokens(b"/* why */ int x = 1; // note\n"))

    def test_literal_comment_markers_remain_significant(self):
        """URLs and comment-like text inside literals are program data, not comments."""
        self.assertNotEqual(cpp_tokens(b'auto s="https://one";'), cpp_tokens(b'auto s="https://two";'))

    def test_raw_literal_comment_markers_remain_significant(self):
        """Multiline raw string contents must survive lexical comparison intact."""
        self.assertNotEqual(cpp_tokens(b'auto s=R"tag(//one\n/*x*/)tag";'), cpp_tokens(b'auto s=R"tag(//two\n/*x*/)tag";'))

    def test_joining_operators_is_a_code_change(self):
        """Whitespace removal must not hide a different C++ operator."""
        self.assertNotEqual(cpp_tokens(b"x + +y;"), cpp_tokens(b"x++y;"))

    def test_macro_name_spacing_is_significant(self):
        """A space before '(' changes a function-like macro into an object-like macro."""
        self.assertNotEqual(cpp_tokens(b"#define F(x) x\n"), cpp_tokens(b"#define F (x) x\n"))

    def test_directive_newline_is_significant(self):
        """Moving code into a macro body is a behavior change despite identical words."""
        self.assertNotEqual(cpp_tokens(b"#define X 1\nint y;"), cpp_tokens(b"#define X 1 int y;\n"))

    def test_raw_literal_backslash_newline_is_not_spliced(self):
        """Raw literal line continuations are data, so changes must be rejected."""
        before = b'auto s=R"(a' + bytes([92, 10]) + b'b)";'
        self.assertNotEqual(cpp_tokens(before), cpp_tokens(b'auto s=R"(ab)";'))

    def test_multiline_comment_in_macro_does_not_end_directive(self):
        """C++ replaces a block comment with one space before directive parsing."""
        self.assertEqual(cpp_tokens(b"#define X 1\n"),
                         cpp_tokens(b"#define X /* why\ncontinued */ 1\n"))

    def test_stringification_sensitive_separator_change_is_rejected(self):
        """Macro stringification can observe whitespace even when tokens match."""
        self.assertNotEqual(cpp_tokens(b"#define S(x) #x\nS(a+b)"),
                            cpp_tokens(b"#define S(x) #x\nS(a + b)"))


@unittest.skipUnless(shutil.which("git"), "Git is required to exercise history comparison")
class SourceHistoryTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root / "src").mkdir()
        (self.root / "res").mkdir()
        (self.root / "src/function.cpp").write_text("int value() { return 1; }\n")
        (self.root / "res/data.bin").write_bytes(b"original")
        (self.root / "CMakeLists.txt").write_text("project(fixture)\n")
        self.git("init", "--quiet")
        self.git("add", ".")
        self.git("-c", "user.name=Fixture", "-c", "user.email=fixture@example.invalid", "commit", "--quiet", "-m", "test fixture")
        self.base = self.git("rev-parse", "HEAD").decode().strip()

    def git(self, *args):
        return subprocess.run(["git", "-C", str(self.root), *args], capture_output=True, check=True).stdout

    def test_history_accepts_comment_only_changes(self):
        """The guard must compare against real historical contents, not the working tree twice."""
        (self.root / "src/function.cpp").write_text("// rationale\nint value() { return 1; }\n")
        self.assertEqual(check_sources(self.root, self.base), (1, 2))

    def test_history_rejects_changed_return_value(self):
        """A changed numeric result must violate the no-code-change promise."""
        (self.root / "src/function.cpp").write_text("int value() { return 2; }\n")
        with self.assertRaisesRegex(ValueError, "non-comment"):
            check_sources(self.root, self.base)

    def test_history_rejects_deleted_source(self):
        """Deleting a production file cannot be disguised as documentation work."""
        (self.root / "src/function.cpp").unlink()
        with self.assertRaisesRegex(ValueError, "file set"):
            check_sources(self.root, self.base)

    def test_history_rejects_changed_asset(self):
        """Tests and documentation must leave original binary resources intact."""
        (self.root / "res/data.bin").write_bytes(b"modified")
        with self.assertRaisesRegex(ValueError, "asset/build"):
            check_sources(self.root, self.base)


if __name__ == "__main__":
    unittest.main()