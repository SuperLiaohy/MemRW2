//
// Created by liaohy on 12/17/25.
//

#pragma once
#include "Die.h"

namespace dwarf {
class TypeDie {
public:
    TypeDie()=delete;
    TypeDie(Die*other) : die(other) {}

    std::optional<std::string> getName();
    std::optional<std::size_t> getSize();

    template<typename Callback>
    void recursion(Callback callback, void* userData) noexcept {die->recursion(callback,userData);};

    template<typename StartCallback, typename EndCallback>
    void recursion(StartCallback startCallback, EndCallback endCallback, void* userData) noexcept {die->recursion(startCallback,endCallback,userData);};

    Die* die;

};

}
