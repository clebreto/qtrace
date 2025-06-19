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

project(qtrNewtonWriter-mt)

## #################################################################
## Build rules
## #################################################################

# Create executable
add_executable(${PROJECT_NAME} main.cpp)

# Link Qt6 libraries and dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Concurrent
    qtrCore
)

# Set C++ standard for this target
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

## #################################################################
## Install rules - targets
## #################################################################

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

######################################################################
### CMakeLists.txt ends here
