//
// Created by liaohy on 1/5/26.
//

#pragma once
#include <memory>
#include <qtextstream.h>
#include <QVector>
#include <vector>

#include "VariTree.hpp"

class VariComponent;
class DisplayPluginInterface {
public:
    DisplayPluginInterface(QString settingHeader);
    void detachSelf();

    virtual void onPluginRunning(qreal runTime) = 0;
    virtual void onPluginStart(){};
    virtual void onPluginEnd(){};
    virtual bool onGeneratePluginSetting(QTextStream& stream){return true;};
    virtual bool onLoadPluginSetting(QTextStream& stream){return true;};

    QStringList reloadVari();

    void pushUnit(const QString&name , const QString& type, std::size_t address, std::size_t size);
    void pushUnit(const std::shared_ptr<VariComponent>& vari);

    void eraseUnit(int index);
    void eraseUnits(QSet<int>& indexes);
    void eraseAll(QSet<int>& indexes);

    std::vector<std::shared_ptr<VariComponent>> variContainer;
    QString settingHeader;
};