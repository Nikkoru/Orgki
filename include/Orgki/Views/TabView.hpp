#pragma once

#include <qtconfigmacros.h>
#include <qtmetamacros.h>
#include <qwidget.h>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QPushButton;
class QVBoxLayout;
QT_END_NAMESPACE

class TabView : public QWidget {
    Q_OBJECT
private:
    QVBoxLayout* m_MainLayout;
    std::vector<QWidget> m_Widgets;
    QTabWidget* m_Tabs;
    QPushButton* m_NewButton;
public:
    TabView();
    ~TabView() = default;
};
