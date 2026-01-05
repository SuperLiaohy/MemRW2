//
// Created by liaohy on 1/5/26.
//

#include "VariComponent.hpp"
#include "DisplayPluginInterface.hpp"

VariComponent::VariComponent(const std::shared_ptr<VariNode> &v, DisplayPluginInterface *interface)  : parent(interface),selected(true),info(v) {
    if (parent)parent->vari=this;
}
