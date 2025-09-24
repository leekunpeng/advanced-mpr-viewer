#include <QApplication>
#include <QOpenGLWidget>
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QMessageBox>
#include <QDebug>

#include "version.h"
#include "ui/MainWindow.h"

void setupOpenGLFormat()
{
    // Set up OpenGL 3.3 Core Profile as default
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4); // Enable multisampling
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    
    QSurfaceFormat::setDefaultFormat(format);
    
    qDebug() << "OpenGL format set: " << format.version() << format.profile();
}

void setupDarkTheme(QApplication& app)
{
    // Apply dark theme
    app.setStyle(QStyleFactory::create("Fusion"));
    
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    
    app.setPalette(palette);
    
    qDebug() << "Dark theme applied";
}

bool checkOpenGLSupport()
{
    // Create a temporary OpenGL widget to check support
    QOpenGLWidget testWidget;
    testWidget.show();
    testWidget.makeCurrent();
    
    auto* context = testWidget.context();
    if (!context || !context->isValid()) {
        QMessageBox::critical(nullptr, "OpenGL Error", 
                             "Failed to create OpenGL 3.3+ context.\n\n"
                             "Please ensure your graphics drivers are updated and support OpenGL 3.3 or higher.");
        return false;
    }
    
    auto format = context->format();
    qDebug() << "OpenGL context created successfully:";
    qDebug() << "  Version:" << format.majorVersion() << "." << format.minorVersion();
    qDebug() << "  Profile:" << (format.profile() == QSurfaceFormat::CoreProfile ? "Core" : 
                                format.profile() == QSurfaceFormat::CompatibilityProfile ? "Compatibility" : "NoProfile");
    
    testWidget.hide();
    
    if (format.majorVersion() < 3 || (format.majorVersion() == 3 && format.minorVersion() < 3)) {
        QMessageBox::critical(nullptr, "OpenGL Version Error", 
                             QString("OpenGL %1.%2 is not sufficient.\n\n"
                                   "Advanced MPR Viewer requires OpenGL 3.3 or higher.\n"
                                   "Please update your graphics drivers.")
                             .arg(format.majorVersion()).arg(format.minorVersion()));
        return false;
    }
    
    return true;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application properties
    app.setApplicationName(AdvancedMPRViewer::PROJECT_NAME_STR);
    app.setApplicationVersion(AdvancedMPRViewer::PROJECT_VERSION_STR);
    app.setOrganizationName("Advanced MPR Viewer Project");
    app.setOrganizationDomain("advanced-mpr-viewer.org");
    
    // Setup OpenGL format before creating any OpenGL widgets
    setupOpenGLFormat();
    
    // Setup dark theme
    setupDarkTheme(app);
    
    // Check OpenGL support
    if (!checkOpenGLSupport()) {
        return 1;
    }
    
    // Create and show main window
    MainWindow window;
    window.show();
    
    return app.exec();
}