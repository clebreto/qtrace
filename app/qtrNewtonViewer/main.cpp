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
    
    // 1. Define possible QML module paths
    QStringList possiblePaths = {
        // Development build path
        QCoreApplication::applicationDirPath() + "/../qml",
        QCoreApplication::applicationDirPath() + "/../lib/qml",
        // Installed path
        QCoreApplication::applicationDirPath() + "/../Resources/qml",
        "/usr/local/qml",
        "/usr/local/lib/qml",
        "/usr/local/opt/qt/lib/qml",
        "/opt/homebrew/lib/qml",
        "/opt/homebrew/opt/qt@6/lib/qml",
        "/usr/local/opt/qt@6/lib/qml",
        // Fallback to install path
        "/app/install/qml"
    };
    
    // Add the application's directory as a fallback
    possiblePaths.prepend(QCoreApplication::applicationDirPath());
    
    // Make paths absolute and remove duplicates
    QSet<QString> uniquePaths;
    for (const QString &path : qAsConst(possiblePaths)) {
        QString absPath = QDir(path).absolutePath();
        if (QFileInfo::exists(absPath)) {
            uniquePaths.insert(absPath);
        }
    }
    
    // Add the paths to the engine
    bool foundQtrQuick = false;
    for (const QString &path : qAsConst(uniquePaths)) {
        engine.addImportPath(path);
        qCDebug(lcViewer) << "Added import path:" << path;
        
        // Check if QtrQuick exists in this path
        QString qtrQuickPath = path + "/QtrQuick";
        if (QFileInfo::exists(qtrQuickPath)) {
            qCDebug(lcViewer) << "Found QtrQuick module at:" << qtrQuickPath;
            foundQtrQuick = true;
            
            // Check for plugin files (support both .so and .dylib extensions)
            QStringList pluginFiles = QDir(qtrQuickPath).entryList({"libqtrQuick.*"}, QDir::Files);
            if (!pluginFiles.isEmpty()) {
                qCDebug(lcViewer) << "Found QtrQuick plugin:" << pluginFiles;
            } else {
                qCWarning(lcViewer) << "No QtrQuick plugin found in:" << qtrQuickPath;
                qCDebug(lcViewer) << "Directory contents:" << QDir(qtrQuickPath).entryList();
            }
        }
    }
    
    if (!foundQtrQuick) {
        qCWarning(lcViewer) << "QtrQuick module not found in any of the search paths";
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
            !engine.importPathList().contains(absEnvPath)) {
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
