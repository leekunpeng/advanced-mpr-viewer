#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void about();
    void openDicomDirectory();
};