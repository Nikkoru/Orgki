#include "Orgki/Views/TreeView.hpp"

#include <QVBoxLayout>
#include <qlabel.h>
#include <qnamespace.h>
#include <qwidget.h>

TreeView::TreeView() : m_MainLayout(new QVBoxLayout(this)) {
    auto* label = new QLabel(this);
    label->setText(tr("treeview"));
    label->setAlignment(Qt::AlignCenter);

    m_MainLayout->addWidget(label);

    setLayout(m_MainLayout);
    QWidget* opts = new QWidget;

}
