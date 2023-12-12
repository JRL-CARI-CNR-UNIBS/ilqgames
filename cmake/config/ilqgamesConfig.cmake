
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was ilqgamesConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

get_filename_component(ilqgames_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
include(CMakeFindDependencyMacro)

find_dependency(Eigen3      REQUIRED)
find_dependency(gflags      REQUIRED)
find_dependency(glog        REQUIRED)
find_dependency(OpenGL      REQUIRED)
find_dependency(GLUT        REQUIRED)
find_dependency(PkgConfig   REQUIRED)
find_dependency(Boost       REQUIRED)
# find_dependency(imgui       REQUIRED)

include("${ilqgames_CMAKE_DIR}/ilqgamesTargets.cmake")

set(ilqgames_FOUND TRUE)

set(ilqgames_LIBRARIES ilqgames)

check_required_components(ilqgames)
