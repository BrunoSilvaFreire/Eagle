cmake_minimum_required(VERSION 3.14)

project(eagle)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(EG_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(EG_EXTERNAL_PATH ${EG_ROOT_PATH}/external)


option(BUILD_EG_ENGINE "Build eagle engine library" OFF)
option(BUILD_EG_EDITOR "Build eagle editor executable (requires engine)" OFF)

add_definitions(-DPROJECT_ROOT="${EG_ROOT_PATH}/data")
if(MSVC)
    add_definitions(-D_ENABLE_EXTENDED_ALIGNED_STORAGE)
endif(MSVC)

add_subdirectory(${EG_EXTERNAL_PATH}/spdlog ${CMAKE_BINARY_DIR}/spdlog)

set(EAGLE_SOURCE
        eagle/application.cpp
        eagle/layer_stack.cpp
        eagle/input.cpp
        eagle/input_layer.cpp
        eagle/random.cpp
        eagle/timer.cpp
        eagle/file_system.cpp
        eagle/events/event.cpp

        eagle/renderer/vertex_layout.cpp
        eagle/renderer/graphics_buffer.cpp
        eagle/renderer/vulkan/vulkan_context.cpp
        eagle/renderer/vulkan/vulkan_helper.cpp
        eagle/renderer/vulkan/vulkan_global_definitions.cpp
        eagle/renderer/vulkan/vulkan_shader.cpp
        eagle/renderer/vulkan/vulkan_buffer.cpp
        eagle/renderer/vulkan/vulkan_vertex_buffer.cpp
        eagle/renderer/vulkan/vulkan_index_buffer.cpp
        eagle/renderer/vulkan/vulkan_uniform_buffer.cpp
        eagle/renderer/vulkan/vulkan_descriptor_set.cpp
        eagle/renderer/vulkan/vulkan_descriptor_set_layout.cpp
        eagle/renderer/vulkan/vulkan_texture.cpp
        eagle/renderer/vulkan/spirv_reflect.cpp
        eagle/renderer/vulkan/vulkan_converter.cpp
        eagle/renderer/vulkan/vulkan_command_buffer.cpp
        eagle/renderer/vulkan/vulkan_cleaner.cpp
        eagle/renderer/vulkan/vulkan_compute_shader.cpp
        eagle/renderer/vulkan/vulkan_shader_utils.cpp
        eagle/renderer/vulkan/vulkan_storage_buffer.cpp
        eagle/renderer/vulkan/vulkan_framebuffer.cpp
        eagle/renderer/vulkan/vulkan_render_pass.cpp
        eagle/renderer/vulkan/vulkan_image.cpp

#        eagle/core/source/renderer/vulkan/platform/android/VulkanContextAndroid.cpp
        )


set(EAGLE_PLATFORM_LIBS)

if (WIN32)
    message(STATUS "Eagle -- Including win32 sources")

#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR -DVK_NO_PROTOTYPES")

    set(GLFW_STANDALONE OFF)
    set(GLFW_INSTALL OFF)
    set(GLFW_BUILD_DOCS OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/glfw ${CMAKE_CURRENT_BINARY_DIR}/glfw)
    find_package(Vulkan REQUIRED)

    list(APPEND EAGLE_SOURCE
            eagle/platform/desktop/desktop_window_glfw.cpp
            eagle/platform/desktop/desktop_application.cpp
            eagle/platform/desktop/desktop_file_system.cpp
            eagle/renderer/vulkan/platform/desktop/vulkan_context_glfw.cpp
            )

    list(APPEND EAGLE_PLATFORM_LIBS glfw Vulkan::Vulkan)

elseif(ANDROID)
    message(STATUS " Eagle - Including android sources")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_ANDROID_KHR -DVK_NO_PROTOTYPES")


    add_library(native_app_glue STATIC
            ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
            )
    target_include_directories(native_app_glue PUBLIC
            ${ANDROID_NDK}/sources/android/native_app_glue
            )

    find_library(log-lib log)


    list(APPEND EAGLE_SOURCE
            eagle/platform/android/android_window.cpp
            eagle/platform/android/android_application.cpp
            eagle/platform/android/android_file_system.cpp
            eagle/renderer/vulkan/platform/android/vulkan_context_android.cpp
            eagle/renderer/vulkan/platform/android/vulkan_wrapper.cpp
            )


    list(APPEND EAGLE_PLATFORM_LIBS
            android
            native_app_glue
            ${log-lib}
            )
else()
    message(ERROR "Unsuported platform!")
endif()

set(EAGLE_ALL ${EAGLE_SOURCE})

add_library(eagle STATIC "${EAGLE_ALL}")

target_include_directories(
        eagle
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
        eagle
        PROPERTIES
        LINKER_LANGUAGE CXX
        CXX_STANDARD 17
)

include(cmake/Eagle.cmake)

define_file_basename_for_sources(eagle)

target_link_libraries(eagle PUBLIC spdlog ${EAGLE_PLATFORM_LIBS})
