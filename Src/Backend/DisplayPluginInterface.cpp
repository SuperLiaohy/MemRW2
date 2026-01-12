//
// Created by liaohy on 1/5/26.
//

#include "DisplayPluginInterface.hpp"
#include "Backend.hpp"
#include "VariComponent.hpp"

DisplayPluginInterface::DisplayPluginInterface(VariComponent *vari) {
    Backend::instance().pushPlugin(this);
}

DisplayPluginInterface::~DisplayPluginInterface() {
    Backend::instance().erasePlugin(this);
}

QStringList DisplayPluginInterface::reloadVari() {
    QStringList failedList;
    for (auto& vari: variContainer) {
        QModelIndex index = Backend::instance().findNode(vari->getName());
        if (!index.isValid()) {failedList.append(vari->getName());continue;}
        vari->updateFromModel(index);
    }
    return failedList;
}

void DisplayPluginInterface::pushUnit(const QString&name , const QString& type, std::size_t address, std::size_t size) {
    auto vari = std::make_shared<VariComponent>(name,type,address,size);
    variContainer.push_back(vari);
}

void DisplayPluginInterface::pushUnit(const std::shared_ptr<VariComponent>& vari) {
    variContainer.push_back(vari);
}

void DisplayPluginInterface::eraseUnit(int index) {
    variContainer.erase(variContainer.begin()+index);
}

void DisplayPluginInterface::eraseUnits(QSet<int>& indexes) {
    int index=0;
    erase_if(variContainer, [&indexes,&index](auto&){return indexes.contains(index++);});
}

