# DFK added.
project(imgui
        LANGUAGES CXX C)
cmake_minimum_required(VERSION 2.6.2)

# Add an option for building a shared library
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include_directories(${imgui_SOURCE_DIR}/include ${imgui_SOURCE_DIR}/../glfw/include)
link_directories(${imgui_BINARY_DIR})

#find_package(PkgConfig REQUIRED)
#pkg_search_module(GLFW3 REQUIRED glfw)
#include_directories()

find_package( glfw3 REQUIRED )

find_package( OpenGL REQUIRED )
include_directories(SYSTEM ${OPENGL_INCLUDE_DIRS})

find_package( GLUT REQUIRED )
include_directories(SYSTEM ${GLUT_INCLUDE_DIRS})

file(GLOB_RECURSE imgui_srcs ${imgui_SOURCE_DIR}/src/*.c*)

# Add the library
set(imgui_LIBRARY_DIR ${CMAKE_INSTALL_PREFIX}/lib)

if(BUILD_SHARED_LIBS)
    add_library(${PROJECT_NAME} SHARED ${imgui_srcs})
    set(imgui_LIBRARIES ${imgui_LIBRARY_DIR}/libimgui.so PARENT_SCOPE)
else()
    add_library(${PROJECT_NAME} STATIC ${imgui_srcs})
    set(imgui_LIBRARIES ${imgui_LIBRARY_DIR}/libimgui.a PARENT_SCOPE)
endif()

target_link_libraries(${PROJECT_NAME} ${imgui_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} glfw)

# Install the library
set(imgui_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include/${PROJECT_NAME})
set(imgui_EXPORT_DIR  ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME})
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets 
        LIBRARY DESTINATION ${imgui_LIBRARY_DIR}
        ARCHIVE DESTINATION ${imgui_LIBRARY_DIR}
        INCLUDES DESTINATION ${imgui_INCLUDE_DIR})

# Export the targets
install(EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        DESTINATION ${imgui_EXPORT_DIR})

# Install the headers
install(DIRECTORY ${imgui_SOURCE_DIR}/include/
        DESTINATION ${imgui_INCLUDE_DIR}
        FILES_MATCHING PATTERN "*.h")


# Package config
include(CMakePackageConfigHelpers)
set(INCLUDE_DIR include)

configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in 
  ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${imgui_EXPORT_DIR}
  PATH_VARS INCLUDE_DIR
)

install(FILES
  ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}Config.cmake
  DESTINATION ${imgui_EXPORT_DIR}
)