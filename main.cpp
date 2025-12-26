#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFileSystemModel>
#include <qqmlcontext.h>

#include "Src/Line/LineGroupModel.hpp"
#include "Src/Line/LineModel.hpp"
#include "Src/Variable/TreeModel.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    auto variModel = std::make_unique<TreeModel>(std::make_shared<VariTree>());
    engine.rootContext()->setContextProperty("myTreeModel", variModel.get());

    auto& lineGroupModel = LineGroupModel::instance();
    lineGroupModel.appendGroup("first");
    lineGroupModel.appendGroup("second");

    auto& lineModel1 = lineGroupModel[0];
    auto& lineModel2 = lineGroupModel[1];
    lineModel1->appendLine("123","blue");
    lineModel2->appendLine("35","red");

    auto& l1 = lineModel1->operator[](0);
    l1->write(QPointF(2,2));
    l1->write(QPointF(4,4));
    l1->write(QPointF(10,10));
    l1->write(QPointF(20,20));
    l1->write(QPointF(50,20));
    l1->write(QPointF(20,50));
    l1->write(QPointF(200,150));
    auto& l2 = lineModel2->operator[](0);
    l2->write(QPointF(22,200));
    l2->write(QPointF(43,45));
    l2->write(QPointF(110,20));
    l2->write(QPointF(24,70));
    l2->write(QPointF(51,50));
    l2->write(QPointF(40,20));
    l2->write(QPointF(50,140));

    engine.loadFromModule("MemRWApp", "Main");
    // engine.load(QUrl::fromLocalFile("../qml/Main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
