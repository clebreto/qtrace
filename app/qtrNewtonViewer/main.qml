// Version: $Id$
//
//
// Commentary:
//
//
// Change Log:
//
//
// Code:
import QtQuick 2.15
import QtQuick.Window 2.15
import QtrQuick 1.0  // Module name matches qmldir

Window {
    id: mainWindow
    visible: true
    width: 800
    height: 600
    title: "Newton Fractal Viewer"
    color: "#1e1e1e"
    
    // Debug information overlay
    Rectangle {
        id: debugInfo
        anchors {
            top: parent.top
            left: parent.left
            margins: 10
        }
        width: 300
        height: debugColumn.height + 20
        color: "#80000000"
        radius: 5
        border.color: "#666"
        border.width: 1
        
        Column {
            id: debugColumn
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: 10
            }
            spacing: 5
            
            Text {
                text: "Debug Information"
                color: "white"
                font.bold: true
                font.pixelSize: 14
            }
            
            Text {
                text: "Window Size: " + mainWindow.width + "x" + mainWindow.height
                color: "#aaa"
                font.pixelSize: 12
            }
            
            Text {
                id: canvasInfo
                text: "Canvas: Not Ready"
                color: "#aaa"
                font.pixelSize: 12
            }
            
            Text {
                id: threadInfo
                text: "Threads: 0"
                color: "#aaa"
                font.pixelSize: 12
            }
            
            Text {
                id: progressInfo
                text: "Progress: 0%"
                color: "#aaa"
                font.pixelSize: 12
            }
        }
    }
    
    Rectangle {
        id: canvasContainer
        anchors.fill: parent
        color: "transparent"
        border.color: "#444"
        border.width: 1
        
        QtrCanvas {
            id: canvas
            anchors.fill: parent
            
            onWidthChanged: {
                console.log("Canvas width changed to:", width);
                canvasInfo.text = "Canvas: " + width + "x" + height + 
                                " (" + Math.round(width) + "x" + Math.round(height) + ")";
            }
            
            onHeightChanged: {
                console.log("Canvas height changed to:", height);
                canvasInfo.text = "Canvas: " + width + "x" + height + 
                                " (" + Math.round(width) + "x" + Math.round(height) + ")";
            }
            
            onCurNumberOfThreadsChanged: {
                console.log("Threads changed to:", canvas.curNumberOfThreads);
                thread_gauge.value = canvas.curNumberOfThreads;
                threadInfo.text = "Threads: " + canvas.curNumberOfThreads + 
                                " (" + thread_gauge.value.toFixed(0) + "%)";
            }
            
            onMinProgressValueChanged: {
                console.log("Min progress:", canvas.minProgressValue);
                progress_gauge.value_min = canvas.minProgressValue;
            }
            
            onMaxProgressValueChanged: {
                console.log("Max progress:", canvas.maxProgressValue);
                progress_gauge.value_max = canvas.maxProgressValue;
            }
            
            onCurProgressValueChanged: {
                console.log("Progress:", canvas.curProgressValue);
                progress_gauge.value = canvas.curProgressValue;
                progressInfo.text = "Progress: " + 
                    Math.round(((canvas.curProgressValue - canvas.minProgressValue) / 
                               (canvas.maxProgressValue - canvas.minProgressValue)) * 100) + "%";
            }
            
            Component.onCompleted: {
                console.log("Canvas component completed");
                canvasInfo.text = "Canvas: " + width + "x" + height + 
                                " (" + Math.round(width) + "x" + Math.round(height) + ")";
            }
            
            // Thread gauge
            QtrGauge {
                id: thread_gauge
                anchors {
                    right: parent.right
                    bottom: parent.bottom
                    margins: 10
                }
                width: 150
                height: 80
                
                value: 0
                value_min: 0
                value_max: canvas.maxNumberOfThreads
                
                caption: "Thread Usage"
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        console.log("Increasing Newton order");
                        canvas.newtonOrder = canvas.newtonOrder + 1;
                    }
                    onDoubleClicked: {
                        console.log("Resetting Newton order to 2");
                        canvas.newtonOrder = 2;
                    }
                }
            }
            
            // Progress gauge
            QtrGauge {
                id: progress_gauge
                anchors {
                    right: thread_gauge.left
                    bottom: parent.bottom
                    margins: 10
                }
                width: 150
                height: 80
                
                value: 0
                value_min: 0
                value_max: 100
                
                caption: "Progress"
            }
        }
    }
    
    // Status bar
    Rectangle {
        id: statusBar
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        height: 20
        color: "#1a1a1a"
        
        Text {
            id: statusText
            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
            color: "#aaa"
            font.pixelSize: 11
            text: "Ready"
        }
        
        Text {
            id: fpsCounter
            anchors {
                right: parent.right
                rightMargin: 10
                verticalCenter: parent.verticalCenter
            }
            color: "#666"
            font.pixelSize: 10
            text: "FPS: --"
        }
    }
    
    // FPS counter
    Timer {
        interval: 1000
        repeat: true
        running: true
        onTriggered: {
            var fps = Math.round(frameCount);
            fpsCounter.text = "FPS: " + fps;
            frameCount = 0;
        }
    }
    
    property int frameCount: 0
    
    // Frame counter for FPS calculation
    Timer {
        interval: 16 // ~60fps
        repeat: true
        running: true
        onTriggered: {
            frameCount++;
        }
    }
}

//
// main.qml ends here
