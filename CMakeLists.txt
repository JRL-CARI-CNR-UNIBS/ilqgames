cmake_minimum_required(VERSION 2.8.7)
cmake_policy (SET CMP0072 OLD) # to suppress warning for developers
cmake_policy (SET CMP0048 NEW) # to allow specifying project version

project(ilqgames
        LANGUAGES C CXX
        VERSION 0.0.1
)
message("Project Version: " ${${PROJECT_NAME}_VERSION})

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Build options.
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCUMENTATION "Build documentation" ON)

# Add cmake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)
message("Cmake module path: ${CMAKE_MODULE_PATH}")

# Add cmake utilities.
include(cmake/Utils.cmake)

# Add dependencies.
include(cmake/Dependencies.cmake)

# Check for C++17 features and enable.
ilqgames_enable_cpp17()

# # Set the build type. Default to Release mode.
# if(NOT CMAKE_BUILD_TYPE)
#   message("Defaulting to building targets in Release mode.")
#   set(CMAKE_BUILD_TYPE Release)
# endif(NOT CMAKE_BUILD_TYPE)

# Set the build type. Default to Debug mode.
if(NOT CMAKE_BUILD_TYPE)
  message("Defaulting to building targets in Debug mode.")
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

# Find and build third party libraries.
# set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) # instruct CMake to use the directories in the linker search path as the runtime search path
add_subdirectory(external)
list(APPEND ilqgames_LIBRARIES ${imgui_LIBRARIES})

# Add the source directories to the search path so cmake can find our headers.
include_directories(${CMAKE_SOURCE_DIR}/include ${imgui_SOURCE_DIR}/include)

# Include imgui cmake targets.
include(/usr/local/lib/cmake/imgui/imguiTargets.cmake)

# Collect source files and build the ilqgames object.
file(GLOB_RECURSE ${PROJECT_NAME}_srcs ${CMAKE_SOURCE_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_srcs})
target_link_libraries(${PROJECT_NAME} PUBLIC ${ilqgames_LIBRARIES} imgui)

# Set compiler constants and expose them.
set(LOG_DIR "${CMAKE_SOURCE_DIR}/logs" CACHE STRING "Logging directory used by the solver to dump results.")
target_compile_definitions(${PROJECT_NAME} PUBLIC ILQGAMES_LOG_DIR="${LOG_DIR}")

# Display compiler constants
get_target_property(DEFS ${PROJECT_NAME} INTERFACE_COMPILE_DEFINITIONS)
message("Compile definitions: " ${DEFS})


# === INSTALLATION === #
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  RUNTIME DESTINATION bin
  INCLUDES DESTINATION include
)

set(EXPORT_CONFIGDIR "lib/cmake/${PROJECT_NAME}")

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
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin/)

# Find and build executables.
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
