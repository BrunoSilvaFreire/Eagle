cmake_minimum_required(VERSION 3.18.4)


set(EG_APP_EXE_NAME triangle)
set(EG_APP_LIB_NAME trianglelib)

#set(CMAKE_CXX11_EXTENSION_COMPILE_OPTION "")

add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/${EG_APP_LIB_NAME})

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

add_library(${EG_APP_EXE_NAME} SHARED
        src/main/cpp/main.cpp
        )

target_link_libraries(${EG_APP_EXE_NAME}
        PUBLIC
        -Wl,--whole-archive
        ${EG_APP_LIB_NAME}
        -Wl,--no-whole-archive
        )