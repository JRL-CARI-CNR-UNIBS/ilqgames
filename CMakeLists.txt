cmake_minimum_required(VERSION 3.0.0)
cmake_policy (SET CMP0072 NEW) # to set OpenGL_GL_PREFERENCE to GLVND (use "OLD" to set to LEGACY)
cmake_policy (SET CMP0048 NEW) # to allow specifying project version

project(ilqgames
  LANGUAGES C CXX
  VERSION 0.0.1
)
message("Project Version: " ${${PROJECT_NAME}_VERSION})

# Build options.
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCUMENTATION "Build documentation" ON)

# Set the build type. Default to Release mode.
if(NOT CMAKE_BUILD_TYPE)
  message("Defaulting to building targets in Release mode.")
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

# Find third party libraries.
find_package(Eigen3 REQUIRED)
find_package(imgui REQUIRED)

# If Gflags has been installed using "sudo apt-get install libgflags-dev", then one must specify the path to the CMake config files.
find_package(gflags REQUIRED PATHS /usr/lib/x86_64-linux-gnu/) 

# If Glog has been installed using "sudo apt-get install libgoogle-glog-dev",
# then one must use PkGConfig since the CMake config files are not provided.
find_package(PkgConfig REQUIRED)
pkg_check_modules(glog REQUIRED libglog)

# Include CMake utilities.
include(cmake/Utils.cmake)

# Check for C++17 support.
ilqgames_enable_cpp17()

# Collect source files and build the ilqgames library.
file(GLOB_RECURSE ${PROJECT_NAME}_srcs ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_srcs})

# Link dependent libraries to the target library ${PROJECT_NAME}.
target_link_libraries(${PROJECT_NAME} PUBLIC imgui gflags glog)

# Add to the target ${PROJECT_NAME} the include directories of the dependencies.
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${EIGEN3_INCLUDE_DIRS}
  ${imgui_INCLUDE_DIRS}
  ${gflags_INCLUDE_DIRS}
  ${glog_INCLUDE_DIRS}
)

# Set compiler constants and expose them.
set(LOG_DIR "${CMAKE_SOURCE_DIR}/logs" CACHE STRING "Logging directory used by the solver to dump results.")
target_compile_definitions(${PROJECT_NAME} PUBLIC ILQGAMES_LOG_DIR="${LOG_DIR}")

# Display compiler constants
get_target_property(DEFS ${PROJECT_NAME} INTERFACE_COMPILE_DEFINITIONS)
message("Compile definitions: " ${DEFS})


# === INSTALLATION === #
# Install the library and export the library target.
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

# Install the header files.
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  FILES_MATCHING PATTERN "*.h"
)

set(EXPORT_CONFIGDIR "share/${PROJECT_NAME}/cmake")

install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION ${EXPORT_CONFIGDIR}
)


# === PACKAGING === #
include(CMakePackageConfigHelpers)

set(INCLUDE_DIR include)
set(CONFIG_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake/config")

configure_package_config_file(${CMAKE_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in 
  ${CONFIG_DIR}/${PROJECT_NAME}Config.cmake
  INSTALL_DESTINATION ${EXPORT_CONFIGDIR}
  PATH_VARS INCLUDE_DIR
)

write_basic_package_version_file(${CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  VERSION ${${PROJECT_NAME}_VERSION}
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  ${CONFIG_DIR}/${PROJECT_NAME}Config.cmake
  ${CONFIG_DIR}/${PROJECT_NAME}ConfigVersion.cmake
  DESTINATION ${EXPORT_CONFIGDIR}
)

# Create debian package (optional) using CPack
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "MikFerrari") #required
SET(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/JRL-CARI-CNR-UNIBS/ilqgames.git")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "") # no dependecies from other deb packages
SET(CPACK_OUTPUT_FILE_PREFIX packages) # output directory for deb package
INCLUDE(CPack)


# === ADDITIONAL BUILDS === #
# Set up output directory for executables.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/Debug/)
else(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/Release/)
endif()

# Find and build executables.
message(STATUS "Building executables.")
add_subdirectory(exec)

# Find and build tests.
if (BUILD_TESTS)
  message("Build tests is enabled.")
  add_subdirectory(test)
endif (BUILD_TESTS)

# Find and build documentation.
if (BUILD_DOCUMENTATION)
  message("Build documentation is enabled.")
  add_subdirectory(documentation)
endif (BUILD_DOCUMENTATION)
