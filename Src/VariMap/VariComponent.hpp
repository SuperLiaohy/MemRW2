//
// Created by liaohy on 1/5/26.
//

#pragma once

#include "DisplayPluginInterface.hpp"
#include "VariTree.hpp"

class DisplayPluginInterface;
class VariComponent {
public:
    VariComponent(const std::shared_ptr<VariNode>& v, DisplayPluginInterface* interface);
    DisplayPluginInterface *parent;

    [[nodiscard]] const std::string& getName() const {return info->getName();}
    [[nodiscard]] std::size_t getSize() const {return info->getSize();}
    [[nodiscard]] const std::string& getType() const {return info->getType();}
    [[nodiscard]] std::size_t getAddress() const {return info->getAbsolute();}
private:
    bool selected;
    std::shared_ptr<VariNode> info;
};
