if (GLFW_FOUND)
    return()
endif ()

add_library(GLFW STATIC IMPORTED GLOBAL)
target_include_directories(GLFW
        INTERFACE
        ${CMAKE_HOME_DIRECTORY}/external/glfw/include
        )

message("Including ${CMAKE_HOME_DIRECTORY}/external/glfw/include!")
if (wIN32)
    set_target_properties(GLFW PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glfw/lib-vc2015/glfw3.lib)
endif ()

if (UNIX AND NOT APPLE)
    set_target_properties(GLFW PROPERTIES IMPORTED_LOCATION ${CMAKE_HOME_DIRECTORY}/external/glfw/lib-unix/libglfw3.a)
    find_package(X11 REQUIRED)
    find_package(Threads REQUIRED)
    target_link_libraries(GLFW INTERFACE "${X11_X11_LIB}" "${CMAKE_THREAD_LIBS_INIT}")

    find_library(RT_LIBRARY rt)
    mark_as_advanced(RT_LIBRARY)
    if (RT_LIBRARY)
        target_link_libraries(GLFW INTERFACE "${RT_LIBRARY}")
    endif ()

    find_library(MATH_LIBRARY m)
    mark_as_advanced(MATH_LIBRARY)
    if (MATH_LIBRARY)
        target_link_libraries(GLFW INTERFACE "${MATH_LIBRARY}")
    endif ()
    if (CMAKE_DL_LIBS)
        target_link_libraries(GLFW INTERFACE "${CMAKE_DL_LIBS}")
    endif ()
endif ()

set(GLFW_FOUND True)