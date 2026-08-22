#include "Orgki/Views/TabView.hpp"
#include <qboxlayout.h>
#include <qnamespace.h>
#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qlabel.h>

TabView::TabView() : m_Tabs(new QTabWidget(this)), m_NewButton(new QPushButton(this)), m_MainLayout(new QVBoxLayout(this)) {
    m_MainLayout->addWidget(m_Tabs); 

    m_NewButton->setText(tr("+"));
    m_Tabs->setCornerWidget(m_NewButton);
    m_Tabs->setMovable(true);
    m_Tabs->setTabsClosable(true);

    QWidget* widget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel(this);
    label->setText(tr("test"));
    label->setAlignment(Qt::AlignCenter);
    widget->setLayout(layout);
    layout->addWidget(label);

    m_Tabs->addTab(widget, tr("Tab1"));

    setLayout(m_MainLayout);
}
