#pragma once

#include <QMainWindow>
#include <filesystem>
#include <qtconfigmacros.h>

QT_BEGIN_NAMESPACE
class QSplitter;
class QDockWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    QSplitter* m_Spliter{};
public:
    MainWindow();
    ~MainWindow() = default;

    void LoadSavedState(const std::filesystem::path& path);
private:
    
};
