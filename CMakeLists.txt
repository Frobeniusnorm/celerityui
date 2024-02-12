cmake_minimum_required(VERSION 3.9)
project(celerityui VERSION 0.0.1)
set(TARGET celerityui)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(OpenGL_GL_PREFERENCE LEGACY)
option(BUILD_EXAMPLES "Building example programs" ON)

FILE(GLOB_RECURSE SRCFILES src/*.cpp)

add_library(${TARGET} STATIC ${SRCFILES})

set_property(TARGET celerityui PROPERTY CXX_STANDARD 20)
target_include_directories(celerityui PRIVATE .)

find_package(Threads REQUIRED)
target_link_libraries(${TARGET} Threads::Threads)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(${TARGET} glfw)

find_package(OpenGL REQUIRED)
target_link_libraries(${TARGET} OpenGL::GL)

find_package(GLEW REQUIRED)
target_link_libraries(${TARGET} GLEW)

find_package(glm REQUIRED)
target_link_libraries(${TARGET} glm)

find_package(Freetype REQUIRED)
target_link_libraries(${TARGET} ${FREETYPE_LIBRARIES})
target_include_directories(${TARGET} PRIVATE ${FREETYPE_INCLUDE_DIRS})

set_target_properties(${TARGET} PROPERTIES VERSION ${PROJECT_VERSION} PUBLIC_HEADER "src/celerityui.h")
install(TARGETS ${TARGET}
    EXPORT celerityuiTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

if(${BUILD_EXAMPLES})
	project(simple_window)
	add_executable(simple_window examples/simple_window/simple_window.cpp)
	target_link_libraries(simple_window ${TARGET})
	target_include_directories(simple_window PRIVATE ./src)
endif()
