#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFileSystemModel>
#include <qqmlcontext.h>

#include "Src/Variable/TreeModel.hpp"
#include "Backend.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    Backend::instance().init();
    Backend::instance().registerVariModel(engine);

    engine.loadFromModule("MemRWApp", "Main");
    // engine.load(QUrl::fromLocalFile("../qml/Main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
