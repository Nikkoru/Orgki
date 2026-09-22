#include "Orgki/MainWindow.hpp"
#include "Orgki/Views/TabView.hpp"
#include "Orgki/Views/TreeView.hpp"
#include "libOrgki/Context.hpp"

#include <qboxlayout.h>
#include <qevent.h>
#include <qkeysequence.h>
#include <qlabel.h>
#include <qlayoutitem.h>
#include <qmainwindow.h>
#include <qdockwidget.h>
#include <qnamespace.h>
#include <qsplitter.h>
#include <qstackedlayout.h>
#include <QKeyEvent>
#include <qapplication.h>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenuBar>
#include <qdialog.h>
#include <qwidget.h>

MainWindow::MainWindow(Orgki::Context& ctx, QApplication& app)
    : m_Context(ctx)
    , m_App(app)
    , m_Spliter(new QSplitter(Qt::Orientation::Horizontal, this))
    , m_MainContainer(new QWidget{})
    , m_MainLayout(new QStackedLayout{m_MainContainer})
	, m_TreeView(new TreeView{ m_Context })
	, m_TabView(new TabView)
    , m_CmdWidget(new QWidget{}) {
    _SetActions();
    _SetMenus();

    setFocusPolicy(Qt::StrongFocus);

    m_MainLayout->setStackingMode(QStackedLayout::StackAll);
    m_MainLayout->setContentsMargins(0, 0, 0, 0);

    m_Spliter->addWidget(m_TreeView);
    m_Spliter->addWidget(m_TabView);

	connect(this, &MainWindow::PlansUpdated, m_TreeView, &TreeView::UpdateView);

    QWidget *overlayWidget = new QWidget;
    // Give the overlay a semi-transparent background color
    overlayWidget->setStyleSheet("background-color: rgba(50, 0, 0, 100);"); 

    QVBoxLayout *overlayLayout = new QVBoxLayout(overlayWidget);
    QLabel *overlayLabel = new QLabel("Overlay Notification Panel");
    overlayLabel->setStyleSheet("color: white; font-weight: bold;");
    overlayLayout->addWidget(overlayLabel, 0, Qt::AlignCenter);

    m_MainLayout->addWidget(m_Spliter);
    m_MainLayout->addWidget(overlayWidget);
    m_Depth++;

    m_MainLayout->setCurrentIndex(m_Depth);
    m_MainContainer->setLayout(m_MainLayout);

    setCentralWidget(m_MainContainer);

    setMinimumSize({ 128, 128 });
    resize({ 1280, 720 });
}

void MainWindow::_CreateCmdWidgets() {
    m_CmdWidget = new QWidget{};
    auto* layout = new QVBoxLayout{};
    auto* spacer = new QSpacerItem{ 5, 10, QSizePolicy::Minimum, QSizePolicy::Expanding };
    m_CmdWidget->setStyleSheet("background-color: rgba(0, 0, 0, 150);"); 

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);
    layout->addSpacerItem(spacer);

    m_CmdInput = new QLineEdit{};
    m_CmdInput->installEventFilter(this);

    m_CmdInput->setMaximumHeight(30);
    m_CmdInput->setMinimumHeight(30);
    connect(m_CmdInput, &QLineEdit::textChanged, this, &MainWindow::BufChanged);
    connect(m_CmdInput, &QLineEdit::returnPressed, this, &MainWindow::BufEnterPressed);

    layout->addWidget(m_CmdInput);

    m_CmdWidget->setLayout(layout);
    m_ActiveCmd = true;
}

void MainWindow::_UpdateView(std::optional<Orgki::Command::StatusData> status) {
    if (!status) return;

    emit PlansUpdated();
}

void MainWindow::_SetActions() {
    m_CreateAct = new QAction{
        QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew),
        tr("Create Plan")
    };
    m_CreateAct->setShortcut(QKeySequence::New);
    m_CreateAct->setStatusTip("Creates a new plan");
    connect(m_CreateAct, &QAction::triggered, this, &MainWindow::Create);

    m_OpenAct = new QAction{
        QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen),
        "&Open Plan"
    };
    m_OpenAct->setShortcut(QKeySequence::Open);
    m_OpenAct->setStatusTip("Opens a existing plan");
    connect(m_OpenAct, &QAction::triggered, this, &MainWindow::Open);
    
    m_SaveAct = new QAction{
        QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave),
        "&Save"
    };
    m_SaveAct->setShortcut(QKeySequence::Save);
    m_SaveAct->setStatusTip("Saves the changes made");
    connect(m_SaveAct, &QAction::triggered, this, &MainWindow::Save);

    m_UndoAct = new QAction{
        QIcon::fromTheme(QIcon::ThemeIcon::EditUndo),
        "&Undo"
    };
    m_UndoAct->setShortcut(QKeySequence::Undo);
    m_UndoAct->setStatusTip("Undo the changes made");
    connect(m_UndoAct, &QAction::triggered, this, &MainWindow::Undo);

    m_RedoAct = new QAction{
        QIcon::fromTheme(QIcon::ThemeIcon::EditRedo),
        "&Redo"
    };
    m_RedoAct->setShortcut(QKeySequence::Redo);
    m_RedoAct->setStatusTip("Redo the changes made");
    connect(m_RedoAct, &QAction::triggered, this, &MainWindow::Redo);

    m_CommandAct = new QAction{
        QIcon::fromTheme(QIcon::ThemeIcon::InputKeyboard),
        "&Command Line"
    };
    m_CommandAct->setShortcut(QKeySequence{ Qt::Key_Colon });
    m_CommandAct->setStatusTip("Opens the command line");
    connect(m_CommandAct, &QAction::triggered, this, &MainWindow::OpenCmd);

    m_InfoAct = new QAction{
        QIcon::fromTheme(QIcon::ThemeIcon::DialogInformation),
        "&Help"
    };
    m_InfoAct->setShortcut(QKeySequence{ Qt::Key_F1 });
    m_InfoAct->setStatusTip("Get info about the many things of Orgki");
    connect(m_InfoAct, &QAction::triggered, this, &MainWindow::Info);
}

void MainWindow::_SetMenus() {
    m_FileMenu = menuBar()->addMenu("&File");
    m_FileMenu->addAction(m_CreateAct);
    m_FileMenu->addAction(m_OpenAct);

    m_EditMenu = menuBar()->addMenu("&Edit");
    m_EditMenu->addAction(m_SaveAct);
    m_EditMenu->addAction(m_UndoAct);
    m_EditMenu->addAction(m_RedoAct);

    m_HelpMenu = menuBar()->addMenu("&Help");
    m_HelpMenu->addAction(m_CommandAct);
    m_HelpMenu->addAction(m_InfoAct);
}

bool MainWindow::focusNextPrevChild(bool next) {
    return false; // Prevents Qt from intercepting Tab and Shift+Tab
}

bool MainWindow::event(QEvent* event) {
    if (event->type() == QEvent::KeyPress && m_ActiveCmd) {
        auto keyEvent = static_cast<QKeyEvent*>(event);

        if (keyEvent->key() == Qt::Key_Backtab) {
            if (!m_Context.parser.HasActiveMatch()) {
                return QMainWindow::event(event);
            }
            else {
                auto value = m_Context.parser.GetPreviousMatch();
                if (value.has_value())
                    m_CmdBuffer = value.value().command;

				m_CmdInput->setText(m_CmdBuffer.c_str());
                return true;
            }
        }
        else if (keyEvent->key() == Qt::Key_Tab) {
			if (!m_Context.parser.HasActiveMatch()) {
				m_Context.parser.FindMatches(m_CmdBuffer);
                return true;
			}
            else {
                auto value = m_Context.parser.GetNextMatch();
                if (value.has_value())
                    m_CmdBuffer = value.value().command;
            }

            m_CmdInput->setText(m_CmdBuffer.c_str());
            return true;
        }
    }
    
    return QMainWindow::event(event);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        if (m_MainLayout->count() > 1) {
			auto* item = m_MainLayout->takeAt(m_Depth);
			if (!item) return;
            m_Depth--;

			if (m_ActiveCmd)
				m_ActiveCmd = false;
            if (auto* widget = item->widget())
                widget->deleteLater();

            m_MainLayout->setCurrentIndex(m_Depth);

            delete item;
            return;
        }
    }
    else if (event->key() == Qt::Key_Colon) {
        if (!m_ActiveCmd) {
            _CreateCmdWidgets();
            m_MainLayout->addWidget(m_CmdWidget);
            m_MainLayout->setCurrentIndex(++m_Depth);
			m_CmdInput->setFocus();
            return;
        }
    }
    else if (m_ActiveCmd) {
        if (event->key() == Qt::Key_Up) {
             m_CmdBuffer = m_Context.parser.GetNextHistory();
             m_CmdInput->setText(m_CmdBuffer.c_str());
        }
        else if (event->key() == Qt::Key_Down) {
             m_CmdBuffer = m_Context.parser.GetNextHistory();
             m_CmdInput->setText(m_CmdBuffer.c_str());

        }
        else if (event->key() == Qt::Key_Tab) {
        }
        else {
            m_Context.parser.ClearMatches();
        }
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
    QMainWindow::keyReleaseEvent(event);
}

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
    QMainWindow::contextMenuEvent(event);
}

void MainWindow::Open() {

}

void MainWindow::Create() {

}

void MainWindow::Save() {

}

void MainWindow::Undo() {

}

void MainWindow::Redo() {

}

void MainWindow::Info() {

}

void MainWindow::OpenCmd() {

}

void MainWindow::BufChanged(const QString& text) {
    m_CmdBuffer = text.toStdString();
}

void MainWindow::BufEnterPressed() {
    auto status = m_Context.parser.Parse(m_CmdBuffer);
    m_CmdInput->clear();

    if (!status.has_value()) {
        if (status.error() == Orgki::Parser::Status::REQUEST_EXIT)
            QApplication::quit();

		auto* dialog = new QMessageBox{ this };
        dialog->setWindowTitle("Parser Failed");
        dialog->setText("Parser has failed with the following status:");
        dialog->setInformativeText(Orgki::Parser::StatusToString(status.error()).c_str());

        dialog->setIcon(QMessageBox::Critical);
        dialog->setStandardButtons(QMessageBox::Ok);

        dialog->exec();

        return;
    }
    else if (status.value().status != Orgki::Command::Status::OK) {
		auto* dialog = new QMessageBox{ this };
        dialog->setWindowTitle("Command Failed");
        dialog->setText(
            std::format(
                "ID: {}\nThe executed command failed with the following msg:",
                Orgki::Parser::StatusToString(status.value().status)
            ).c_str()
        );
        dialog->setInformativeText(status.value().msg.c_str());

        dialog->setIcon(QMessageBox::Critical);
        dialog->setStandardButtons(QMessageBox::Ok);

        dialog->exec();

        return;
    }

    _UpdateView(status.value());
}
