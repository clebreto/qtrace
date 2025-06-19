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
    qtrQuick  # Add qtrQuick library for QML components
)

# Set C++ standard for this target
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Set QML import paths for the target
set(QML_IMPORT_PATHS "${CMAKE_INSTALL_PREFIX}/qml")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # In debug builds, also look in the build directory
    set(QML_IMPORT_PATHS "${CMAKE_CURRENT_BINARY_DIR}/qml;${QML_IMPORT_PATHS}")
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE
    QML_IMPORT_PATH="${QML_IMPORT_PATHS}"
)

# Set runtime environment for the target
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_DEBUGGER_ENVIRONMENT "QML_IMPORT_PATH=${QML_IMPORT_PATHS}"
    XCODE_ATTRIBUTE_QML_IMPORT_PATH "${QML_IMPORT_PATHS}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.qtrNewtonViewer"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
)

## #################################################################
## Install rules - targets
## #################################################################

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

######################################################################
### CMakeLists.txt ends here
