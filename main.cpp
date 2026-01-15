#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFileSystemModel>
#include <qqmlcontext.h>
#include <qqml.h>
#include "Src/Variable/TreeModel.hpp"
#include "Backend.hpp"
#include "Src/ExTable/ExTableModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/qt/qml/MemRWApp/app-icon.jpg"));

    Backend::instance().init();
    QQmlApplicationEngine engine;

    Backend::instance().registerVariModel(engine);

    engine.loadFromModule("MemRWApp", "Main");

    if (engine.rootObjects().isEmpty()) return -1;

    app.exec();

    return 0;
}
