#include "runtime_options.h"
#include "validation.h"
#include <fstream>
#include <sstream>
#include <map>
#include <stdexcept>
#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
namespace runtime {
namespace {
std::string trim(const std::string& s) {
    const auto a=s.find_first_not_of(" \t\r\n");
    return a==s.npos?std::string{}:s.substr(a,s.find_last_not_of(" \t\r\n")-a+1);
}
float number(const std::string& s) {
    std::size_t end=0;float value=std::stof(s,&end);
    if(end!=s.size() || !std::isfinite(value))throw std::invalid_argument("Invalid finite number: "+s);
    return value;
}
int dimension(const std::string& s) {
    std::size_t end=0;int value=std::stoi(s,&end);
    if(end!=s.size() || value<0 || value>16384)throw std::invalid_argument("Window dimensions must be integers in [0,16384]");
    return value;
}
void apply(Options& out,const std::string& key,const std::string& value) {
    if(key=="width")out.width=dimension(value);
    else if(key=="height")out.height=dimension(value);
    else if(key=="window_scale")out.windowScale=number(value);
    else if(key=="camera_speed")out.cameraSpeed=number(value);
    else if(key=="mouse_sensitivity")out.mouseSensitivity=number(value);
    else if(key=="assets")out.assets=std::filesystem::u8path(value);
    else if(key=="exit_after")out.exitAfter=number(value);
    else throw std::invalid_argument("Unknown configuration option: "+key);
}
}
Options parseOptions(const std::vector<std::string>& args) {
    Options out;std::map<std::string,std::string> overrides;std::filesystem::path config;
    for(std::size_t i=0;i<args.size();++i) {
        const auto& arg=args[i];
        if(arg=="--help" || arg=="-h") { out.help=true;continue; }
        if(arg=="--graphics-check") { out.graphicsCheck=true;continue; }
        const std::map<std::string,std::string> keys={{"--width","width"},{"--height","height"},{"--window-scale","window_scale"},{"--camera-speed","camera_speed"},{"--mouse-sensitivity","mouse_sensitivity"},{"--assets","assets"},{"--exit-after","exit_after"}};
        if(arg!="--config" && !keys.count(arg))throw std::invalid_argument("Unknown argument: "+arg);
        if(++i==args.size())throw std::invalid_argument("Missing value for "+arg);
        if(arg=="--config")config=std::filesystem::u8path(args[i]);
        else overrides[keys.at(arg)]=args[i];
    }
    if(!config.empty()) {
        std::ifstream file(config);if(!file)throw std::runtime_error("Cannot open configuration: "+config.u8string());
        std::string line;unsigned int n=0;
        while(std::getline(file,line)) {
            ++n;line=trim(line);if(line.empty() || line.front()=='#')continue;
            auto eq=line.find('=');if(eq==line.npos)throw std::invalid_argument("Expected key=value at configuration line "+std::to_string(n));
            auto key=trim(line.substr(0,eq)),value=trim(line.substr(eq+1));
            if(value.empty())throw std::invalid_argument("Empty configuration value at line "+std::to_string(n));
            apply(out,key,value);
        }
        if(file.bad())throw std::runtime_error("Cannot read configuration: "+config.u8string());
        if(!out.assets.empty() && out.assets.is_relative())out.assets=config.parent_path()/out.assets;
    }
    for(const auto& kv:overrides)apply(out,kv.first,kv.second);
    validation::require((out.width==0)==(out.height==0),"Set both width and height, or use 0 for both");
    validation::require(out.windowScale>0 && out.windowScale<=1 && out.cameraSpeed>=0 && out.mouseSensitivity>=0 && out.exitAfter>=0,"Invalid scale, speed, sensitivity or exit interval");
    return out;
}
std::filesystem::path executablePath() {
#ifdef _WIN32
    std::wstring buffer(32768,L'\0');auto size=GetModuleFileNameW(nullptr,buffer.data(),static_cast<DWORD>(buffer.size()));
    if(!size || size==buffer.size())throw std::runtime_error("Cannot find executable location");
    buffer.resize(size);return std::filesystem::path(buffer);
#elif defined(__APPLE__)
    uint32_t size=0;_NSGetExecutablePath(nullptr,&size);std::string buffer(size,'\0');
    if(_NSGetExecutablePath(buffer.data(),&size)!=0)throw std::runtime_error("Cannot find executable location");
    return std::filesystem::weakly_canonical(buffer.c_str());
#else
    return std::filesystem::read_symlink("/proc/self/exe");
#endif
}
std::filesystem::path resolveAssets(const std::filesystem::path& exe,const std::filesystem::path& explicitRoot) {
    auto valid=[](const auto& p) {return std::filesystem::is_directory(p/"shaders") && std::filesystem::is_directory(p/"meshes") && std::filesystem::is_directory(p/"textures");};
    if(!explicitRoot.empty()) {
        if(!valid(explicitRoot))throw std::runtime_error("Asset directory must contain shaders, meshes and textures: "+explicitRoot.u8string());
        return std::filesystem::weakly_canonical(explicitRoot);
    }
    for(const auto& p:{exe.parent_path()/"res",exe.parent_path()/"../res",exe.parent_path()/"../share/royalgreen/res"})
        if(valid(p))return std::filesystem::weakly_canonical(p);
    throw std::runtime_error("Cannot find assets beside executable; keep res/ with the package or pass --assets PATH");
}
const char* usage() {return
"RoyalGreen\n"
"  --assets PATH              Resource directory (default: res beside executable)\n"
"  --config FILE              Read key=value configuration; CLI values override it\n"
"  --width N --height N       Window size; 0/0 uses monitor scale (default)\n"
"  --window-scale VALUE       Monitor fraction, in (0,1] (default 0.7)\n"
"  --camera-speed VALUE       Scene units/second (default 1.35)\n"
"  --mouse-sensitivity VALUE  Radians per cursor unit (default 0.001)\n"
"  --exit-after SECONDS       Optional automatic close (0 disables it)\n"
"  --graphics-check          Run context/resource regression checks and exit\n"
"  --help                    Show this text without a graphics driver\n";}
}
