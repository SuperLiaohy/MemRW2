//
// Created by liaohy on 1/5/26.
//

#include "VariComponent.hpp"
#include "DisplayPluginInterface.hpp"

VariComponent::VariComponent(VariNode* node)  : active(true),
name(node->getName()), size(node->getSize()), address(node->getAbsolute()) {
    type = Type::UINT32;
}
