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

    virtual void updateData(qreal runTime) = 0;

    void pushUnit(const std::shared_ptr<VariNode>& node);
    void eraseUnit(int index);

    std::vector<std::shared_ptr<VariComponent>> variContainer;
};