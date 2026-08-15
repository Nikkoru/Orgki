#include <print>
#include <QApplication>

#include "Orgki/MainWindow.hpp"

int main(int argc, char** argv) {
    QApplication app{argc, argv};
    MainWindow win{};
    std::println("Orgki or smth");

    win.show();

    return app.exec();
}
