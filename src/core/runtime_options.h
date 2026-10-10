#pragma once
#include <filesystem>
#include <string>
#include <vector>
namespace runtime {
struct Options {
    int width=0,height=0;
    float windowScale=0.7f,cameraSpeed=1.35f,mouseSensitivity=0.001f;
    std::filesystem::path assets;
    bool help=false,graphicsCheck=false;
    float exitAfter=0;
};
Options parseOptions(const std::vector<std::string>& arguments);
std::filesystem::path executablePath();
std::filesystem::path resolveAssets(const std::filesystem::path& executable,const std::filesystem::path& explicitRoot={});
const char* usage();
}
