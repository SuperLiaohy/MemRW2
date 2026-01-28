//
// Created by liaohy on 12/17/25.
//

#include "VariDie.hpp"
#include "TypeDie.hpp"
namespace dwarf{
std::optional<Die> VariDie::getTypeDie() {
    return die->getTypeDie();
}

std::optional<std::size_t> VariDie::getLoc() {
    Attr attr;
    Error error;
    std::size_t addr = 0;

    int res = dwarf_attr(die->die, DW_AT_location, attr.ptr(), error.ptr());
    if (res != DW_DLV_OK) return std::nullopt;
    res = dwarf_formudata(*attr.ptr(), &addr, error.ptr());

    if (res != DW_DLV_OK) {
        error.clear();
        auto form = attr.getForm();

        Dwarf_Block *block;
        Dwarf_Ptr block_ptr = nullptr;
        Dwarf_Unsigned len = 0;

        if (form == DW_FORM_exprloc) {
            res = dwarf_formexprloc(*attr.ptr(), &len, &block_ptr, error.ptr());
            if (res != DW_DLV_OK) return std::nullopt;
        } else if (form == DW_FORM_block1 || form == DW_FORM_block2 ||
                   form == DW_FORM_block4 || form == DW_FORM_block) {
            res = dwarf_formblock(*attr.ptr(), &block, error.ptr());
            if (res != DW_DLV_OK) return std::nullopt;
            block_ptr = block->bl_data;
            len = block->bl_len;
        } else if (form == DW_FORM_data4 || form == DW_FORM_data8) {
            return std::nullopt;
        } else {
            return std::nullopt;
        }
        Dwarf_Half opcode = 0;
        if (len > 0) {
            opcode = reinterpret_cast<uint8_t *>(block_ptr)[0];
            if (opcode == DW_OP_addr) {
                if (len == 5) {
                    addr = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(block_ptr) + 1);
                }
            } else if (DW_OP_plus_uconst) {
                if (len <= 5) {
                    addr = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(block_ptr) + 1);
                    for (int i = len-1; i < 4; ++i) {
                        addr &= ~(0xff<<(8*i));
                    }
                } else {
                    return std::nullopt;
                }
            }  else {
                return std::nullopt;
            }
        }
    }
    return addr;
}

std::optional<std::size_t> VariDie::getMemLoc() {
    Error error;
    Attr attr;
    Dwarf_Unsigned location;
    auto res = dwarf_attr(die->die, DW_AT_data_member_location, attr.ptr(), error.ptr());
    if (res != DW_DLV_OK) return std::nullopt;
    error.clear();
    res = dwarf_formudata(*attr.ptr(), &location, error.ptr());
    if (res != DW_DLV_OK) {
        error.clear();
        auto form = attr.getForm();
        Dwarf_Block *block;
        Dwarf_Ptr block_ptr = nullptr;
        Dwarf_Unsigned len = 0;

        if (form == DW_FORM_exprloc) {
            res = dwarf_formexprloc(*attr.ptr(), &len, &block_ptr, error.ptr());
            if (res != DW_DLV_OK) return std::nullopt;
        } else if (form == DW_FORM_block1 || form == DW_FORM_block2 ||
                   form == DW_FORM_block4 || form == DW_FORM_block) {
            res = dwarf_formblock(*attr.ptr(), &block, error.ptr());
            if (res != DW_DLV_OK) return std::nullopt;
            block_ptr = block->bl_data;
            len = block->bl_len;
        } else if (form == DW_FORM_data4 || form == DW_FORM_data8) {
            return std::nullopt;
        } else {
            return std::nullopt;
        }

        Dwarf_Half opcode = 0;

        if (len > 0) {
            opcode = reinterpret_cast<uint8_t *>(block_ptr)[0];
            if (opcode == DW_OP_addr) {
                if (len == 5) {
                    location = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(block_ptr) + 1);
                }
            } else if (DW_OP_plus_uconst) {
                if (len <= 5) {
                    location = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(block_ptr) + 1);
                    for (int i = len-1; i < 4; ++i) {
                        location &= ~(0xff<<(8*i));
                    }
                } else {
                    return std::nullopt;
                }
            } else {
                return std::nullopt;
            }
        } else {
            return std::nullopt;
        }
    }
    return location;
}

std::optional<std::string> VariDie::getName() {
    return die->getName();
}
}
