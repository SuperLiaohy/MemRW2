//
// Created by liaohy on 12/11/25.
//

#pragma once
#include "Dwarf.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <libdwarf.h>
#include <dwarf.h>
#ifdef __cplusplus
}
#endif

namespace dwarf {

class Dwarf;
using Tag = Dwarf_Half;
using Form = Dwarf_Half;
using FormClass = Dwarf_Form_Class;
using Symbol = Dwarf_Half;
class Error {
public:
    Error() : error(nullptr) {}
    ~Error();

    static void initDwarf(Dwarf* file) {dwarfFile=file;}

    Dwarf_Error* ptr() {return &error;}
    bool clear();
    void showMsg();
    void autoHandle();
    static Dwarf_Debug dbg() {return dwarfFile->dbg;};
private:
    static volatile Dwarf* dwarfFile;
    Dwarf_Error error;
};
// typedef struct {
//     using s = std::size_t;
//     s size;
// } a;

inline bool isSpaceTypeTag(Tag tag) noexcept {
    if (tag==DW_TAG_class_type||tag==DW_TAG_structure_type||tag==DW_TAG_union_type) {
        return true;
    }
    return false;
}

inline bool isComplexTypeTag(Tag tag) noexcept {
    if (tag==DW_TAG_class_type||tag==DW_TAG_structure_type||tag==DW_TAG_union_type||
        tag==DW_TAG_inheritance) {
        return true;
    }
    return false;
}

inline bool isTypeTag(Tag tag) noexcept {
    if (tag==DW_TAG_base_type||tag==DW_TAG_enumeration_type||
        tag==DW_TAG_class_type||tag==DW_TAG_structure_type||tag==DW_TAG_union_type||
        tag==DW_TAG_inheritance) {
        return true;
    }
    return false;
}

}