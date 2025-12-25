//
// Created by liaohy on 12/11/25.
//

#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <variant>

#include "Utils.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libdwarf.h>
#include <dwarf.h>
#ifdef __cplusplus
}
#endif


namespace dwarf {
class Die;
class Attr {
    friend Die;
public:

    Attr() : attr(nullptr) {}
    Attr(Dwarf_Attribute attr) : attr(attr) {}
    ~Attr() {if (attr!=nullptr) dwarf_dealloc_attribute(attr);}

    [[nodiscard]] std::optional<Form> getForm();
    [[nodiscard]] static std::string formStr(Form form);

    [[nodiscard]] std::optional<FormClass> getFormClass(Die& die);
    [[nodiscard]] std::optional<Symbol> getSymbol();
    [[nodiscard]] static std::string symbolStr(Symbol symbol);

    std::optional<std::string> getString();
    [[nodiscard]] Dwarf_Attribute* ptr() {return &attr;}

private:
    Dwarf_Attribute attr;
};
}
