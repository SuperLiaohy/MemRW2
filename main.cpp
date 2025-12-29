#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFileSystemModel>
#include <qqmlcontext.h>

#include "Src/Line/LineGroupModel.hpp"
#include "Src/Line/LineModel.hpp"
#include "Src/Variable/TreeModel.hpp"
#include "Backend.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    auto variModel = std::make_unique<TreeModel>(std::make_shared<VariTree>());
    engine.rootContext()->setContextProperty("myTreeModel", variModel.get());

    Backend::instance().init();

    engine.loadFromModule("MemRWApp", "Main");
    // engine.load(QUrl::fromLocalFile("../qml/Main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
