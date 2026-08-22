#pragma once

#include <qtconfigmacros.h>
#include <qtmetamacros.h>
#include <qwidget.h>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class TreeView : public QWidget {
    Q_OBJECT
private:
    QVBoxLayout* m_MainLayout{};
public:
    TreeView();
    ~TreeView() = default;
};
