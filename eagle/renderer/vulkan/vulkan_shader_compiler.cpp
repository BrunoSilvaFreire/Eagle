//
// Created by Novak on 16/06/2019.
//

#include "vulkan_shader_compiler.h"
#include "eagle/log.h"

#include <iostream>
#include <eagle/file_system.h>

namespace eagle {

bool VulkanShaderCompiler::m_glslangIntitialized = false;

std::vector<uint32_t> VulkanShaderCompiler::compile_glsl(const std::string &filename, ShaderStage stage) {

    //initializes glslang
    if (!m_glslangIntitialized) {
        if (!glslang::InitializeProcess()){
            throw std::runtime_error("Failed to initialize vulkan shader compiler!");
        }
        m_glslangIntitialized = true;
    }

    EShLanguage shaderStage = get_shader_stage(stage);

    //creates shader object based on shader stage
    glslang::TShader shader(shaderStage);


    std::string glslInput = FileSystem::instance()->read_text(filename);

    const char* cstrInput = glslInput.data();

    shader.setStrings(&cstrInput, 1);

    //environment definitions
    int ClientInputSemanticsVersion = 100; // maps to, say, #define VULKAN 100
    glslang::EShTargetClientVersion VulkanClientVersion = glslang::EShTargetVulkan_1_0;
    glslang::EShTargetLanguageVersion TargetVersion = glslang::EShTargetSpv_1_0;

    shader.setEnvInput(glslang::EShSourceGlsl, shaderStage, glslang::EShClientVulkan, ClientInputSemanticsVersion);
    shader.setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
    shader.setEnvTarget(glslang::EShTargetSpv, TargetVersion);

    TBuiltInResource resources = DefaultTBuiltInResource;
    EShMessages messages = EShMsgDefault;

    const int DefaultVersion = 100;


    //preprocessing of the glsl shader (includes all get_paths into the actual glsl string)
    DirStackFileIncluder includer;

    std::string path = get_file_path(filename);
    includer.pushExternalLocalDirectory(path);

    std::string preprocessedGLSL;

    if (!shader.preprocess(&resources, DefaultVersion, ENoProfile, false, false, messages, &preprocessedGLSL, includer)) {
        EG_CRITICAL("eagle", "GLSL Preprocessing Failed for: {0}\n Log: {1}\n DebugLog: {2}", filename, shader.getInfoLog(), shader.getInfoDebugLog());
    }

    //updates shader strings with preprocessed glsl file
    const char* preprocessedCStr = preprocessedGLSL.c_str();
    shader.setStrings(&preprocessedCStr, 1);

    //parses the shader
    if (!shader.parse(&resources, 100, false, messages)) {
        EG_CRITICAL("eagle", "GLSL Parsing Failed for: {0}\n Log: {1}\n DebugLog: {2}", filename, shader.getInfoLog(), shader.getInfoDebugLog());
    }

    glslang::TProgram program;
    program.addShader(&shader);

    //links program with shader
    if(!program.link(messages)) {
        EG_CRITICAL("eagle", "GLSL Linking Failed for: {0}\n Log: {1}\n DebugLog: {2}", filename, shader.getInfoLog(), shader.getInfoDebugLog());
    }

    //converts glslang program to spirv format
    std::vector<uint32_t> spirv;
    spv::SpvBuildLogger logger;
    glslang::SpvOptions spvOptions;
    glslang::GlslangToSpv(*program.getIntermediate(shaderStage), spirv, &logger, &spvOptions);
    if (!logger.getAllMessages().empty()){
        EG_CRITICAL("eagle", "Failed to convert glsl code to spirv code! Log: {0}", logger.getAllMessages());
    }
    return spirv;
}

std::string VulkanShaderCompiler::get_file_path(const std::string &file) {
    size_t found = file.find_last_of("/\\");
    return file.substr(0,found);
}

std::string VulkanShaderCompiler::get_suffix(const std::string &name) {
    const size_t pos = name.rfind('.');
    return (pos == std::string::npos) ? "" : name.substr(name.rfind('.') + 1);
}

EShLanguage VulkanShaderCompiler::get_shader_stage(ShaderStage stage) {
    switch(stage){
        case ShaderStage::VERTEX:return EShLangVertex;
        case ShaderStage::FRAGMENT:return EShLangFragment;
        case ShaderStage::COMPUTE:return EShLangCompute;
        case ShaderStage::GEOMETRY:return EShLangGeometry;
        case ShaderStage::TESSALATION_CONTROL:return EShLangTessControl;
        case ShaderStage::TESSALATION_EVALUATE:return EShLangTessEvaluation;
    }
}

}
