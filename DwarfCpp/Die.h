//
// Created by liaohy on 12/10/25.
//

#pragma once
#include <optional>
#include <string>
#include <vector>

#include "Attr.h"
#include "Utils.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libdwarf.h>
#include <Dwarf.hpp>
#ifdef __cplusplus
}
#endif

namespace dwarf {
class Die {
    friend class Attr;
    friend class VariDie;
    friend class TypeDie;
public:
    Die() = delete;
    Die(Dwarf_Die die) : die(die) {}
    Die(const Die&) = delete;
    Die operator=(const Die&) = delete;
    Die(Die&&other) noexcept {die = other.die;other.die=nullptr;}
    Die& operator=(Die&&other) noexcept {if (&other!=this) {this->~Die();die = other.die;other.die=nullptr;} return *this;};
    ~Die() {if (die!=nullptr) dwarf_dealloc_die(die);}

    bool operator==(nullptr_t) const {return die==nullptr;}

    std::optional<Die> getChild() noexcept;
    std::optional<Die> getSib() noexcept;
    template<typename StartCallback>
    void recursion(StartCallback startCallback, void* userData) noexcept;
    template<typename StartCallback, typename EndCallback>
    void recursion(StartCallback startCallback, EndCallback endCallback, void* userData) noexcept;

    std::optional<std::string> getName();
    std::optional<Die> getTypeDie();
    std::optional<Die> getDeepTypeDie();

    std::optional<std::size_t> getArrayLen();

    [[nodiscard]] std::optional<Tag> getTag() noexcept;
    static std::string tagStr(Tag tag) noexcept;

    std::vector<Attr> getAttrList() noexcept;
    void displayAttrList();
protected:
    Dwarf_Die die;
};


template<typename StartCallback>
void Die::recursion(StartCallback startCallback, void *userData) noexcept {
    auto child = this->getChild();
    while (child!=std::nullopt) {
        if(startCallback(*child, userData))
            child->recursion(startCallback, userData);
        child = child->getSib();
    }
}

template<typename StartCallback, typename EndCallback>
void Die::recursion(StartCallback startCallback, EndCallback endCallback, void *userData) noexcept {
    auto child = this->getChild();
    while (child!=std::nullopt) {
        if(startCallback(*child, userData))
            child->recursion(startCallback, endCallback, userData);
        endCallback(*child, userData);
        child = child->getSib();
    }
}
}
