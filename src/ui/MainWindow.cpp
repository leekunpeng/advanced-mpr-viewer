#include "MainWindow.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include "version.h"
#include "core/DicomSeriesManager.h"

MainWindow::MainWindow(QWidget* parent)
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
    
    auto* statusLabel = new QLabel("Ready to load DICOM data...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("font-size: 14px; margin: 20px;");

    layout->addStretch();
    layout->addWidget(welcomeLabel);
    layout->addWidget(versionLabel);
    layout->addWidget(statusLabel);
    layout->addStretch();

    // Set up menus
    // File menu
    auto* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("&Open DICOM Directory...", [this]() {
        openDicomDirectory();
    })->setShortcut(QKeySequence::Open);
    fileMenu->addSeparator();
    fileMenu->addAction("E&xit", QKeySequence::Quit, this, &QWidget::close);

    // View menu
    auto* viewMenu = menuBar()->addMenu("&View");
    viewMenu->addAction("&Axial View", [this]() {
        statusBar()->showMessage("Axial View - Not implemented yet", 2000);
    });
    viewMenu->addAction("&Coronal View", [this]() {
        statusBar()->showMessage("Coronal View - Not implemented yet", 2000);
    });
    viewMenu->addAction("&Sagittal View", [this]() {
        statusBar()->showMessage("Sagittal View - Not implemented yet", 2000);
    });

    // Tools menu
    auto* toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction("&Window/Level", [this]() {
        statusBar()->showMessage("Window/Level - Not implemented yet", 2000);
    });
    toolsMenu->addAction("&ROI Tools", [this]() {
        statusBar()->showMessage("ROI Tools - Not implemented yet", 2000);
    });

    // Help menu
    auto* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction("&About", [this]() { about(); });
}

void MainWindow::about()
{
    QMessageBox::about(this, "About Advanced MPR Viewer",
                      QString("<h3>Advanced MPR Viewer v%1</h3>"
                              "<p>A professional medical imaging viewer with Multi-Planar Reconstruction (MPR), "
                              "PET/CT fusion, and advanced analysis tools.</p>"
                              "<p>Built with Qt %2 and OpenGL 3.3+</p>"
                              "<p>Copyright © 2024 Advanced MPR Viewer Project</p>")
                      .arg(AdvancedMPRViewer::PROJECT_VERSION_STR, QT_VERSION_STR));
}

void MainWindow::openDicomDirectory()
{
    QString directory = QFileDialog::getExistingDirectory(
        this,
        "Select DICOM Directory",
        QString(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );
    
    if (directory.isEmpty()) {
        return;
    }
    
    statusBar()->showMessage("Scanning DICOM directory...", 5000);
    
    // Scan directory for DICOM series
    auto seriesList = DicomSeriesManager::scanDirectory(directory.toStdString());
    
    if (seriesList.empty()) {
        QMessageBox::warning(this, "No DICOM Series Found", 
                           QString("No valid DICOM series found in directory:\n%1\n\nError: %2")
                           .arg(directory, QString::fromStdString(DicomSeriesManager::getLastError())));
        statusBar()->showMessage("No DICOM series found", 2000);
        return;
    }
    
    // For now, load the first series found
    const auto& firstSeries = seriesList[0];
    statusBar()->showMessage(QString("Loading series: %1...").arg(QString::fromStdString(firstSeries.seriesDescription)), 5000);
    
    Volume3D volume = DicomSeriesManager::loadSeries(firstSeries);
    
    if (!volume.isValid()) {
        QMessageBox::critical(this, "DICOM Loading Error",
                            QString("Failed to load DICOM series.\n\nError: %1")
                            .arg(QString::fromStdString(DicomSeriesLoader::getLastError())));
        statusBar()->showMessage("DICOM loading failed", 2000);
        return;
    }
    
    // Display success message with volume information
    QString message = QString("DICOM Series Loaded Successfully!\n\n"
                             "Series: %1\n"
                             "Modality: %2\n" 
                             "Dimensions: %3 × %4 × %5\n"
                             "Spacing: %6, %7, %8 mm\n"
                             "Origin: %9, %10, %11 mm\n"
                             "Value Range: %12 to %13\n"
                             "Total Voxels: %14")
                     .arg(QString::fromStdString(volume.seriesDescription))
                     .arg(QString::fromStdString(volume.modality))
                     .arg(volume.width).arg(volume.height).arg(volume.depth)
                     .arg(volume.spacing[0], 0, 'f', 3).arg(volume.spacing[1], 0, 'f', 3).arg(volume.spacing[2], 0, 'f', 3)
                     .arg(volume.origin[0], 0, 'f', 1).arg(volume.origin[1], 0, 'f', 1).arg(volume.origin[2], 0, 'f', 1)
                     .arg(volume.vmin, 0, 'f', 1).arg(volume.vmax, 0, 'f', 1)
                     .arg(volume.getTotalVoxels());
    
    QMessageBox::information(this, "DICOM Loaded", message);
    
    statusBar()->showMessage(QString("Loaded: %1 (%2 × %3 × %4)")
                           .arg(QString::fromStdString(volume.modality))
                           .arg(volume.width).arg(volume.height).arg(volume.depth), 
                           10000);
    
    qDebug() << "Volume geometry validation:";
    qDebug() << "  Orthonormal basis:" << (volume.isOrthonormal() ? "PASS" : "FAIL");
    qDebug() << "  Row direction:" << volume.rowDir[0] << volume.rowDir[1] << volume.rowDir[2];
    qDebug() << "  Col direction:" << volume.colDir[0] << volume.colDir[1] << volume.colDir[2];
    qDebug() << "  Slice direction:" << volume.sliceDir[0] << volume.sliceDir[1] << volume.sliceDir[2];
}