//
// Created by liaohy on 1/5/26.
//

#pragma once
#include <memory>
#include <QVector>
#include <vector>

#include "VariTree.hpp"

class VariComponent;
class DisplayPluginInterface {
public:
    DisplayPluginInterface(VariComponent* vari=nullptr);
    virtual ~DisplayPluginInterface();

    virtual void onPluginRunning(qreal runTime) = 0;
    virtual void onPluginStart(){};
    virtual void onPluginEnd(){};
    QStringList reloadVari();

    void pushUnit(const QString&name , const QString& type, std::size_t address, std::size_t size);
    void pushUnit(const std::shared_ptr<VariComponent>& vari);

    void eraseUnit(int index);
    void eraseUnits(QSet<int>& indexes);
    void eraseAll(QSet<int>& indexes);

    std::vector<std::shared_ptr<VariComponent>> variContainer;
};