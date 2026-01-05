//
// Created by liaohy on 1/5/26.
//

#pragma once
#include <vector>

#include "VariComponent.hpp"


class VariMap {
public:
    void add();
    void erase();
private:
    std::vector<std::shared_ptr<VariComponent>> container;
};
