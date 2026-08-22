#include "Orgki/MainWindow.hpp"
#include "Orgki/Views/TabView.hpp"
#include "Orgki/Views/TreeView.hpp"

#include <qmainwindow.h>
#include <qdockwidget.h>
#include <qnamespace.h>
#include <qsplitter.h>

#include <qdialog.h>

MainWindow::MainWindow() : m_Spliter(new QSplitter(Qt::Orientation::Horizontal, this)) {
    setCentralWidget(m_Spliter);

    m_Spliter->addWidget(new TreeView);
    m_Spliter->addWidget(new TabView);

    setMinimumSize({ 128, 128 });
    resize({ 1280, 720 });

    // QDialog *dialog = new QDialog(this);
    // dialog->setAttribute(Qt::WA_DeleteOnClose);
    // dialog->setModal(true); // Sets application modality
    // dialog->open();
}
