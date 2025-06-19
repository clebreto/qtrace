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

project(qtrNewtonViewer)

## #################################################################
## Build rules
## #################################################################

# Add resources
qt_add_resources(${PROJECT_NAME}_QRC main.qrc)

# Create executable
add_executable(${PROJECT_NAME} main.cpp ${${PROJECT_NAME}_QRC})

# Link Qt6 libraries and dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::QuickControls2
    Qt6::QuickTemplates2
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
