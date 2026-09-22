#pragma once

#include <QMainWindow>
#include <filesystem>
#include <optional>
#include <qapplication.h>
#include <qtconfigmacros.h>
#include <qtmetamacros.h>

#include "libOrgki/Context.hpp"
#include "Orgki/Views/TreeView.hpp"
#include "Orgki/Views/TabView.hpp"

QT_BEGIN_NAMESPACE
class QSplitter;
class QStackedLayout;
class QLineEdit;
class QDockWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
private:
    QMenu* m_FileMenu{};
    QAction* m_CreateAct{};
    QAction* m_OpenAct{};
private:
    QMenu* m_EditMenu{};
    QAction* m_SaveAct{};
    QAction* m_UndoAct{};
    QAction* m_RedoAct{};
private:
    QMenu* m_HelpMenu{};
    QAction* m_CommandAct{};
    QAction* m_InfoAct{};
private:
    Orgki::Context& m_Context;
    QApplication& m_App;
    std::string m_CmdBuffer{};

    QSplitter* m_Spliter{};
    QWidget* m_MainContainer{};
    QStackedLayout* m_MainLayout{};

	TreeView* m_TreeView{};
	TabView* m_TabView{};

    QWidget* m_CmdWidget{};
    QLineEdit* m_CmdInput{};
    bool m_ActiveCmd{ false };
    int m_Depth{};
public:
    MainWindow(Orgki::Context& ctx, QApplication& app);
    ~MainWindow() override = default;

    void LoadSavedState(const std::filesystem::path& path);
signals:
    void PlansUpdated();
private:
    void _CreateCmdWidgets();
    void _UpdateView(std::optional<Orgki::Command::StatusData> status = std::nullopt);

    void _SetActions();
    void _SetMenus();
protected:
    bool focusNextPrevChild(bool next) override;
    bool event(QEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QContextMenuEvent* event) override;
#endif
private slots:
    void Create();
    void Open();
    void Save();
    void Undo();
    void Redo();
    void Info();

    void OpenCmd();

    void BufChanged(const QString& text);
    void BufEnterPressed();
};
