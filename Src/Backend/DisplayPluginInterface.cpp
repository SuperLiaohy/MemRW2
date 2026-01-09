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

void DisplayPluginInterface::pushUnit(const QString&name , const QString& type, std::size_t address, std::size_t size) {
    auto vari = std::make_shared<VariComponent>(name,type,address,size);
    variContainer.push_back(vari);
}

void DisplayPluginInterface::eraseUnit(int index) {
    variContainer.erase(variContainer.begin()+index);
}

void DisplayPluginInterface::eraseUnits(QSet<int>& indexes) {
    int index=0;
    erase_if(variContainer, [&indexes,&index](auto&){return indexes.contains(index++);});
}

