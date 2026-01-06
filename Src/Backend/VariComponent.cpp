//
// Created by liaohy on 1/5/26.
//

#include "VariComponent.hpp"
#include "DisplayPluginInterface.hpp"

VariComponent::VariComponent(const std::shared_ptr<VariNode> &node)  : active(true),
name(node->getName()), size(node->getSize()), type(node->getType()), address(node->getAbsolute()) {
}
