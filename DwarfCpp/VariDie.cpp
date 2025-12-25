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
    int res = dwarf_attr(die->die, DW_AT_location, attr.ptr(), error.ptr());
    if (res != DW_DLV_OK) return std::nullopt;

    // 首先获取属性的形式
    auto form = attr.getForm();

    Dwarf_Block* block;
    Dwarf_Ptr block_ptr = nullptr;
    Dwarf_Unsigned len = 0;

    // 根据属性形式选择不同的处理方法
    if (form == DW_FORM_exprloc) {
        // DWARF4+ 使用 exprloc 形式
        res = dwarf_formexprloc(*attr.ptr(), &len, &block_ptr, error.ptr());
        if (res != DW_DLV_OK) return std::nullopt;
    } else if (form == DW_FORM_block1 || form == DW_FORM_block2 ||
               form == DW_FORM_block4 || form == DW_FORM_block) {
        // DWARF3 使用块形式
        res = dwarf_formblock(*attr.ptr(), &block, error.ptr());
        if (res != DW_DLV_OK) return std::nullopt;
        block_ptr = block->bl_data;
        len = block->bl_len;
    } else if (form == DW_FORM_data4 || form == DW_FORM_data8) {
        return std::nullopt; // 临时返回错误
    } else {
        // 不支持的形式
        return std::nullopt;
    }

    // 解析位置表达式
    std::size_t addr = 0;
    Dwarf_Half opcode = 0;

    if (len > 0) {
        opcode = reinterpret_cast<uint8_t *>(block_ptr)[0];
        if (opcode == DW_OP_addr) { // DW_OP_addr 通常是 0x03
            if (len >= 5) { // 确保有足够的数据
                // 对于 32 位地址
                addr = *reinterpret_cast<uint32_t *>(reinterpret_cast<uint8_t *>(block_ptr) + 1);
            }
            // 如果是 64 位地址，需要使用 uint64_t 来读取
            // if (len >= 9) {
            //     addr = *reinterpret_cast<uint64_t *>(reinterpret_cast<uint8_t *>(block_ptr) + 1);
            // }
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
    if (res != DW_DLV_OK) return std::nullopt;
    return location;
}

std::optional<std::string> VariDie::getName() {
    return die->getName();
}
}
