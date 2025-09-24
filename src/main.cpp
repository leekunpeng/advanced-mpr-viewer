#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>
#include <QSurfaceFormat>
#include <QStyleFactory>
#include <QDirIterator>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>

#include "version.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle(QString("%1 v%2").arg(AdvancedMPRViewer::PROJECT_NAME_STR, AdvancedMPRViewer::PROJECT_VERSION_STR));
        setMinimumSize(800, 600);

        // Create central widget with placeholder content
        auto* centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto* layout = new QVBoxLayout(centralWidget);
        
        auto* welcomeLabel = new QLabel("Advanced MPR Viewer", this);
        welcomeLabel->setAlignment(Qt::AlignCenter);
        welcomeLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin: 20px;");
        
        auto* versionLabel = new QLabel(QString("Version: %1").arg(AdvancedMPRViewer::PROJECT_VERSION_STR), this);
        versionLabel->setAlignment(Qt::AlignCenter);
        versionLabel->setStyleSheet("font-size: 12px; color: gray;");
        
        auto* openglLabel = new QLabel("OpenGL 3.3+ Ready", this);
        openglLabel->setAlignment(Qt::AlignCenter);
        openglLabel->setStyleSheet("font-size: 14px; color: green; margin: 10px;");

        layout->addStretch();
        layout->addWidget(welcomeLabel);
        layout->addWidget(versionLabel);
        layout->addWidget(openglLabel);
        layout->addStretch();

        // Create menu bar
        createMenus();
        
        // Status bar
        statusBar()->showMessage("Ready - Advanced MPR Viewer initialized successfully");
    }

private slots:
    void about()
    {
        QMessageBox::about(this, "About Advanced MPR Viewer",
                          QString("<h3>Advanced MPR Viewer v%1</h3>"
                                  "<p>A professional medical imaging viewer with Multi-Planar Reconstruction (MPR), "
                                  "PET/CT fusion, and advanced analysis tools.</p>"
                                  "<p>Built with Qt %2 and OpenGL 3.3+</p>"
                                  "<p>Copyright © 2024 Advanced MPR Viewer Project</p>")
                          .arg(AdvancedMPRViewer::PROJECT_VERSION_STR, QT_VERSION_STR));
    }

private:
    void createMenus()
    {
        // File menu
        auto* fileMenu = menuBar()->addMenu("&File");
        fileMenu->addAction("&Open DICOM Directory...", [this]() {
            statusBar()->showMessage("DICOM loading not yet implemented", 2000);
        }, QKeySequence::Open);
        fileMenu->addSeparator();
        fileMenu->addAction("E&xit", this, &QWidget::close, QKeySequence::Quit);

        // View menu
        auto* viewMenu = menuBar()->addMenu("&View");
        viewMenu->addAction("&Axial View", [this]() {
            statusBar()->showMessage("Axial view not yet implemented", 2000);
        });
        viewMenu->addAction("&Coronal View", [this]() {
            statusBar()->showMessage("Coronal view not yet implemented", 2000);
        });
        viewMenu->addAction("&Sagittal View", [this]() {
            statusBar()->showMessage("Sagittal view not yet implemented", 2000);
        });

        // Tools menu
        auto* toolsMenu = menuBar()->addMenu("&Tools");
        toolsMenu->addAction("&Window/Level", [this]() {
            statusBar()->showMessage("Window/Level adjustment not yet implemented", 2000);
        });
        toolsMenu->addAction("&ROI Tools", [this]() {
            statusBar()->showMessage("ROI tools not yet implemented", 2000);
        });

        // Help menu
        auto* helpMenu = menuBar()->addMenu("&Help");
        helpMenu->addAction("&About", this, &MainWindow::about);
    }
};

// Include the moc file for Qt's meta-object system
#include "main.moc"

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
}

void setupDarkTheme(QApplication& app)
{
    // Enable dark theme
    app.setStyle(QStyleFactory::create("Fusion"));
    
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    
    app.setPalette(darkPalette);
}

bool checkOpenGLSupport()
{
    // Create a temporary OpenGL widget to check support
    QOpenGLWidget testWidget;
    testWidget.show();
    testWidget.hide();
    
    auto* context = testWidget.context();
    if (!context || !context->isValid()) {
        QMessageBox::critical(nullptr, "OpenGL Error",
                             "Failed to create OpenGL context. Please ensure your graphics drivers support OpenGL 3.3+");
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