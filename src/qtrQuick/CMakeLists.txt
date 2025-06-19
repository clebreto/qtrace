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

project(qtrQuick)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  qtrCanvas.h
  qtrQuickPlugin.h)

set(${PROJECT_NAME}_SOURCES
  qtrCanvas.cpp
  qtrQuickPlugin.cpp)

## #################################################################
## Build rules
## #################################################################

add_definitions(-DQT_PLUGIN)

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS})

# Link Qt6 libraries and dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Concurrent
    Qt6::QuickControls2
    Qt6::QuickTemplates2
    Qt6::Widgets
    qtrCore
)

# Set C++ standard for this target
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

## ###################################################################
## Install rules - targets
## ###################################################################

install(TARGETS ${PROJECT_NAME} DESTINATION lib)

######################################################################
### CMakeLists.txt ends here
