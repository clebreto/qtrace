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

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QLoggingCategory>

// Include the qtrCanvas header
#include <qtrQuick/qtrCanvas.h>

// Enable debug output
Q_LOGGING_CATEGORY(lcViewer, "qtrace.viewer")

// ///////////////////////////////////////////////////////////////////
// qtrRenderer
// ///////////////////////////////////////////////////////////////////
#include "qtrRenderer.h"

// ///////////////////////////////////////////////////////////////////
// main
// ///////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
    // Enable debug output
    qSetMessagePattern("%{time yyyy-MM-dd hh:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{category} %{function}:%{line} - %{message}");
    QLoggingCategory::setFilterRules("qt.*=false\nqt.qpa.*=false");
    
    qCDebug(lcViewer) << "Starting Qtrace Viewer...";
    qCDebug(lcViewer) << "Arguments:" << QCoreApplication::arguments();

    QGuiApplication application(argc, argv);

    if(application.arguments().count() < 2) {
        qCritical() << "Usage:" << argv[0] << "order";
        return 1;
    }

    bool ok;
    int order = application.arguments().at(1).toInt(&ok);
    if (!ok || order < 1) {
        qCritical() << "Invalid order. Must be a positive integer";
        return 1;
    }

// ///////////////////////////////////////////////////////////////////
// Setup qtrRenderer
// ///////////////////////////////////////////////////////////////////
    qCDebug(lcViewer) << "Setting Newton order to:" << order;
    qtrRenderer::newtonOrder = order;

// ///////////////////////////////////////////////////////////////////
// Setup QML engine and import paths
// ///////////////////////////////////////////////////////////////////
    // Set the application name and version
    QGuiApplication::setApplicationName("Newton Fractal Tracer");
    QGuiApplication::setApplicationVersion("1.0");
    
    // Set up the application engine
    QQmlApplicationEngine engine;
    
    // Log initial import paths
    qCDebug(lcViewer) << "Initial QML import paths:" << engine.importPathList();
    
    // Clear all existing import paths to avoid duplicates
    engine.setImportPathList({});
    
    // 1. Define the install path where QML modules are installed
    QString installPath = "/app/install/qml";
    QString absoluteInstallPath = QDir(installPath).absolutePath();
    
    qCDebug(lcViewer) << "Looking for QML modules in:" << absoluteInstallPath;
    
    // 2. Add the install path explicitly
    if (QFileInfo::exists(absoluteInstallPath)) {
        engine.addImportPath(absoluteInstallPath);
        qCDebug(lcViewer) << "Added explicit import path:" << absoluteInstallPath;
        
        // Verify QtrQuick module exists
        QString qtrQuickPath = QDir(absoluteInstallPath + "/QtrQuick").absolutePath();
        if (QFileInfo::exists(qtrQuickPath)) {
            qCDebug(lcViewer) << "Found QtrQuick module at:" << qtrQuickPath;
            
            // Check if the plugin file exists
            QString pluginPath = qtrQuickPath + "/libqtrQuick.so";
            if (QFileInfo::exists(pluginPath)) {
                qCDebug(lcViewer) << "Found QtrQuick plugin at:" << pluginPath;
            } else {
                qCWarning(lcViewer) << "QtrQuick plugin not found at:" << pluginPath;
                // List directory contents for debugging
                QDir dir(qtrQuickPath);
                qCDebug(lcViewer) << "Directory contents:" << dir.entryList();
            }
        } else {
            qCWarning(lcViewer) << "QtrQuick module not found in:" << qtrQuickPath;
        }
    } else {
        qCWarning(lcViewer) << "Install path not found:" << absoluteInstallPath;
    }
    
    // 3. Add system Qt import paths
    QString qtImportsPath = QLibraryInfo::path(QLibraryInfo::QmlImportsPath);
    engine.addImportPath(qtImportsPath);
    qCDebug(lcViewer) << "Added system Qt import path:" << qtImportsPath;
    
    // 4. Add any environment paths that aren't already included
    // but skip any path that might conflict with our install path
    QStringList envPaths = qEnvironmentVariable("QML2_IMPORT_PATH").split(
        QDir::listSeparator(), Qt::SkipEmptyParts);
    for (const QString &envPath : qAsConst(envPaths)) {
        QString absEnvPath = QDir(envPath).absolutePath();
        // Skip /app/qml to avoid conflicts with /app/install/qml
        if (absEnvPath == "/app/qml" || absEnvPath == "/app/qml/") {
            qCDebug(lcViewer) << "Skipping legacy QML path:" << absEnvPath;
            continue;
        }
        
        if (QFileInfo::exists(absEnvPath) && 
            !engine.importPathList().contains(absEnvPath) && 
            absEnvPath != absoluteInstallPath) {
            engine.addImportPath(absEnvPath);
            qCDebug(lcViewer) << "Added environment import path:" << absEnvPath;
        }
    }
    
    // Log final import paths for debugging
    qCDebug(lcViewer) << "Final QML import paths:";
    for (const QString &path : engine.importPathList()) {
        qCDebug(lcViewer) << "  " << path;
    }
    
    // Load the QML file
    qCDebug(lcViewer) << "Loading QML file...";
    const QUrl url(QStringLiteral("qrc:main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &application, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);
    
    // Check for QML loading errors
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML file";
        return -1;
    }
    
    qCDebug(lcViewer) << "Application started successfully";
    
    // Enter the event loop
    return application.exec();
    

}

//
// main.cpp ends here
