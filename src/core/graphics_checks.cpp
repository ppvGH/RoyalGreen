#include "graphics_checks.h"
#include "resource_manager.h"
#include "../graphics/model.h"
#include "../graphics/framebuffer.h"
#include "../graphics/mesh.h"
#include "../app/game2D/sprite_renderer.h"
#include <glad/glad.h>
#include <stdexcept>
#include <utility>
namespace {
void check(bool ok,const char* message) { if(!ok)throw std::runtime_error(message); }
template<typename F>void rejects(F fn,const char* message) { bool threw=false;try { fn(); } catch(const std::exception&) { threw=true; }check(threw,message); }
}
void runGraphicsChecks() {
    unsigned int textureID=0,programID=0,framebufferID=0,vaoID=0;
    {
        unsigned char pixel[]={255,255,255,255};Texture texture(TexParams(1,1),pixel);
        textureID=texture.getID();check(glIsTexture(textureID),"Texture allocation failed");
        Texture moved(std::move(texture));check(!texture.getID() && moved.getID()==textureID,"Texture move duplicated ownership");
        // Reuse moved-from cube owners as color/depth textures. Target state
        // must follow the new allocation rather than the previous cube map.
        Texture cube;cube.genCubeDepthMap();Texture cubeOwner(std::move(cube));
        cube.genColorMap(TexParams(1,1),pixel);cube.bind();
        int bound=0;glGetIntegerv(GL_TEXTURE_BINDING_2D,&bound);
        check(static_cast<unsigned int>(bound)==cube.getID(),"Reused color texture retained cube target");cube.unbind();
        Texture secondCube;secondCube.genCubeDepthMap();Texture secondOwner(std::move(secondCube));
        secondCube.genDepthMap(16,16);secondCube.bind();glGetIntegerv(GL_TEXTURE_BINDING_2D,&bound);
        check(static_cast<unsigned int>(bound)==secondCube.getID(),"Reused depth texture retained cube target");secondCube.unbind();
        Shader shader;shader.linkProgram("#version 330 core\nvoid main(){gl_Position=vec4(0,0,0,1);}","#version 330 core\nout vec4 color;void main(){color=vec4(1);}");
        programID=shader.getID();check(glIsProgram(programID),"Shader allocation failed");
        rejects([&]{shader.linkProgram("#version 330 core\ninvalid syntax", "#version 330 core\nout vec4 color;void main(){color=vec4(1);}");},"Invalid shader did not fail");
        check(shader.getID()==programID && glIsProgram(programID),"Failed reload damaged original program");
        Framebuffer framebuffer(16,16,'c');framebufferID=framebuffer.getID();check(glIsFramebuffer(framebufferID),"Framebuffer allocation failed");
        Mesh mesh;mesh.m_vertices={Vertex({0,0,0},{0,0,1},{0,0}),Vertex({1,0,0},{0,0,1},{1,0}),Vertex({0,1,0},{0,0,1},{0,1})};mesh.m_indices={0,1,2};mesh.setup();vaoID=mesh.getVAO();
        Mesh movedMesh(std::move(mesh));check(mesh.getVAO()==0 && movedMesh.getVAO()==vaoID,"Mesh move duplicated ownership");
        rejects([&]{ Model bad("RoyalGreen-deliberately-missing.obj",&shader); },"Missing model did not fail");
        rejects([&]{ResourceManager::loadTexture("RoyalGreen-deliberately-missing.png",TexParams(),"missing");},"Missing texture did not fail");
        check(!ResourceManager::texturesMap.count("missing"),"Failed texture load inserted a resource");
    }
    check(!glIsTexture(textureID),"Texture was not deleted");check(!glIsProgram(programID),"Program was not deleted");
    check(!glIsFramebuffer(framebufferID),"Framebuffer was not deleted");check(!glIsVertexArray(vaoID),"VAO was not deleted");
    check(glGetError()==GL_NO_ERROR,"Graphics regressions left an OpenGL error");
}
