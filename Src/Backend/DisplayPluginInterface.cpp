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

void DisplayPluginInterface::pushUnit(VariNode* node) {
    auto vari = std::make_shared<VariComponent>(node);
    variContainer.push_back(vari);
}

void DisplayPluginInterface::eraseUnit(int index) {
    variContainer.erase(variContainer.begin()+index);
}

void DisplayPluginInterface::eraseUnits(QSet<int>& indexes) {
    int index=0;
    erase_if(variContainer, [&indexes,&index](auto&){return indexes.contains(index++);});
}

