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

project(qtrCore)

## #################################################################
## Sources
## #################################################################

set(${PROJECT_NAME}_HEADERS
  qtrRenderer.h
  qtrTile.h
  qtrTiler.h)

set(${PROJECT_NAME}_SOURCES
  qtrRenderer.cpp
  qtrTile.cpp
  qtrTiler.cpp)

## #################################################################
## Build rules
## #################################################################

add_library(${PROJECT_NAME} SHARED
  ${${PROJECT_NAME}_SOURCES}
  ${${PROJECT_NAME}_HEADERS}
)

# Link Qt6 libraries and dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Concurrent
)

if(UNIX AND NOT APPLE)
  target_link_libraries(${PROJECT_NAME} PRIVATE bsd)
endif(UNIX AND NOT APPLE)

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
