#include "Orgki/Views/TreeView.hpp"

#include <QVBoxLayout>
#include <qlabel.h>
#include <qlayout.h>
#include <qlayoutitem.h>
#include <qnamespace.h>
#include <qwidget.h>
#include <qtreewidget.h>

namespace{
void clearLayout(QLayout* layout) {
    QLayoutItem* item{};

    while ((item = layout->takeAt(0)) != nullptr) {
        delete item; 
    }
}
}
void TreeView::UpdateView() {
    if (!m_MainLayout->isEmpty()) {
        clearLayout(m_MainLayout);
    }

    if (m_Context.planMgr.IsEmpty()) {
        auto* label = new QLabel(this);
        label->setText(tr("treeview"));
        label->setAlignment(Qt::AlignCenter);

        m_MainLayout->addWidget(label);
    }
    else {
        delete m_Tree;
        m_Tree = new QTreeWidget{ this };

        m_Tree->setColumnCount(2); 
        m_Tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
        m_Tree->setSortingEnabled(true);

        QStringList columnNames{};
        columnNames << "Name" << "N° Tables";
        m_Tree->setHeaderLabels(columnNames);

        for (const auto& plan : m_Context.planMgr.GetAllPlans()) {
            auto count = plan.GetTableCount();

            auto* item = new QTreeWidgetItem{ m_Tree };
            item->setText(0, plan.GetName().c_str());
            item->setText(1, std::format("{}", count).c_str());
        }

        m_MainLayout->addWidget(m_Tree);
    }
}    

TreeView::TreeView(Orgki::Context& ctx)
    : m_Context(ctx)
    , m_Tree(new QTreeWidget{})
    , m_MainLayout(new QVBoxLayout(this)) {
       
    UpdateView();

    setLayout(m_MainLayout);
    // QWidget* opts = new QWidget;
}
