#include "resource_manager.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <memory>
#include <stdexcept>
#ifdef _WIN32
#define STBI_WINDOWS_UTF8
#endif
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include "stb_image.h"
Shader& ResourceManager::loadShader(const std::string& v,const std::string& f,const std::string& name) {
    auto vertex=loadFileToString(v),fragment=loadFileToString(f);
    Shader value;value.linkProgram(vertex,fragment);
    return shadersMap.insert_or_assign(name,std::move(value)).first->second;
}
Shader& ResourceManager::loadShader(const std::string& v,const std::string& g,const std::string& f,const std::string& name) {
    auto vertex=loadFileToString(v),geometry=loadFileToString(g),fragment=loadFileToString(f);
    Shader value;value.linkProgram(vertex,geometry,fragment);
    return shadersMap.insert_or_assign(name,std::move(value)).first->second;
}
Texture& ResourceManager::loadTexture(const std::string& path,TexParams params,const std::string& name) {
    int width=0,height=0,channels=0;stbi_set_flip_vertically_on_load(true);
    std::unique_ptr<unsigned char,decltype(&stbi_image_free)> data(stbi_load(path.c_str(),&width,&height,&channels,STBI_rgb_alpha),stbi_image_free);
    if(!data)throw std::runtime_error("Cannot load texture '"+path+"': "+(stbi_failure_reason()?stbi_failure_reason():"unknown decoder error"));
    if((params.m_width && params.m_width!=width)||(params.m_height && params.m_height!=height))throw std::invalid_argument("Texture dimensions do not match decoded image: "+path);
    params.m_width=width;params.m_height=height;params.m_format=GL_RGBA;
    Texture value(params,data.get());
    return texturesMap.insert_or_assign(name,std::move(value)).first->second;
}
Shader& ResourceManager::getShader(const std::string& name) { auto it=shadersMap.find(name);if(it==shadersMap.end())throw std::runtime_error("Unknown shader: "+name);return it->second; }
Texture& ResourceManager::getTexture(const std::string& name) { auto it=texturesMap.find(name);if(it==texturesMap.end())throw std::runtime_error("Unknown texture: "+name);return it->second; }
std::string ResourceManager::loadFileToString(const std::string& path) {
    std::ifstream file(std::filesystem::u8path(path),std::ios::binary);
    if(!file)throw std::runtime_error("Cannot read shader file: "+path);
    std::ostringstream data;data<<file.rdbuf();
    if(file.bad() || data.str().empty())throw std::runtime_error("Empty or unreadable shader file: "+path);
    return data.str();
}
void ResourceManager::clear() {
    // Unbind before deleting owners; the context must still exist.
    if(glUseProgram)glUseProgram(0);
    texturesMap.clear();shadersMap.clear();
}
