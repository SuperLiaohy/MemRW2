//
// Created by liaohy on 12/17/25.
//

#pragma once

#include "Die.h"
namespace dwarf {
class VariDie {
public:
    VariDie() = delete;
    VariDie(Die*other) : die(other) {}

    std::optional<Die> getTypeDie();
    std::optional<std::size_t> getLoc();
    std::optional<std::size_t> getMemLoc();
    std::optional<std::string> getName();
private:
    Die* die;
};

}