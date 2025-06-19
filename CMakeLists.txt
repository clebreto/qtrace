## Version: $Id$
##
######################################################################
##
### Commentary:
##
######################################################################
##
### Change Log:
##
######################################################################
##
### Code:

cmake_minimum_required(VERSION 3.0.2)

####################################################################

project(qtr)

## #################################################################
## Install prefix
## #################################################################

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "${PROJECT_NAME} install prefix" FORCE)
endif(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)

## #################################################################
## Output directory setup
## #################################################################

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

## #################################################################
## Generate compilation databases
## #################################################################

set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

## #################################################################
## Beautifying
## #################################################################

mark_as_advanced(CMAKE_BUILD_TYPE)
mark_as_advanced(CMAKE_INSTALL_PREFIX)
mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
mark_as_advanced(CMAKE_OSX_SYSROOT)

## #################################################################
## Dependencies - Qt
## #################################################################

# Set minimum Qt version and find packages
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Concurrent Gui Quick QuickControls2 QuickTemplates2 Widgets)

# Set Qt6 as the default
set(QT_DEFAULT_MAJOR_VERSION 6)

# Set C++ standard for all targets
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## #################################################################
## Dependencies - internal
## #################################################################

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/src/qtrCore)
include_directories(${CMAKE_SOURCE_DIR}/src/qtrQuick)

## #################################################################
## Input
## #################################################################

add_subdirectory(src)
add_subdirectory(qml)
add_subdirectory(app)
add_subdirectory(bench)

######################################################################
### CMakeLists.txt ends here
