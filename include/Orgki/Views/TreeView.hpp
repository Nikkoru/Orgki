#pragma once

#include "libOrgki/Context.hpp"
#include <qtconfigmacros.h>
#include <qtmetamacros.h>
#include <qwidget.h>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QTreeWidget;
QT_END_NAMESPACE

class TreeView : public QWidget {
    Q_OBJECT
private:
    Orgki::Context& m_Context;
    QTreeWidget* m_Tree{};
    QVBoxLayout* m_MainLayout{};
signals:
    void ItemSelected();
    void MultipleItemsSelected();
public slots:
    void UpdateView();
public:
    TreeView(Orgki::Context& ctx);
    ~TreeView() = default;
};
