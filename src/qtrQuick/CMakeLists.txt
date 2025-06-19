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

# Install the library
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install QML module
set(QML_MODULE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/qml/QtrQuick")

# Install qmldir and qml files
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../qml/qtrQuick/qmldir.in
    ${CMAKE_CURRENT_SOURCE_DIR}/../qml/qtrQuick/qtrGauge.qml
    DESTINATION ${QML_MODULE_INSTALL_DIR}
    RENAME qmldir
)

# Install the plugin to the QML module directory
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${QML_MODULE_INSTALL_DIR}
    RUNTIME DESTINATION ${QML_MODULE_INSTALL_DIR}
)

# Set the QT_QML_OUTPUT_DIRECTORY to ensure qmlimportscanner finds the module
set(QT_QML_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

######################################################################
### CMakeLists.txt ends here
