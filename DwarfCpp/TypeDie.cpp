//
// Created by liaohy on 12/17/25.
//

#include "TypeDie.hpp"

namespace dwarf {
std::optional<std::string> TypeDie::getName() {
    auto tag = die->getTag().value_or(0);
    if (tag==DW_TAG_typedef) {
        auto realType = die->getTypeDie();
        if (realType==std::nullopt)return "void";
        auto type = TypeDie(&*realType);
        auto n = type.getName();
        if (n==std::nullopt)return std::nullopt;
        if (n->find('<')!=std::string::npos)return die->getName();
        return n;
    }
    auto name = die->getName();
    if (name!=std::nullopt) return name;
    if (tag==DW_TAG_structure_type) return "<anonymous struct>";
    if (tag==DW_TAG_union_type) return "<anonymous union>";
    if (tag==DW_TAG_class_type) return "<anonymous class>";
    if (tag==DW_TAG_const_type||tag==DW_TAG_volatile_type) {
        auto realType = die->getTypeDie();
        if (realType==std::nullopt)return "void";
        auto type = TypeDie(&*realType);
        return type.getName();
    }
    if (tag==DW_TAG_pointer_type) {
        auto realType = die->getTypeDie();
        if (realType==std::nullopt)return "void *";
        auto type = TypeDie(&*realType);
        auto str = type.getName();
        return type.getName().value_or("void")+" *";
    }
    if (tag==DW_TAG_array_type) {
        auto realType = die->getTypeDie();
        if (realType==std::nullopt)return "void";
        auto type = TypeDie(&*realType);
        auto str = type.getName();
        auto number = die->getArrayLen();
        if (number==std::nullopt)return std::nullopt;
        return type.getName().value_or("void")+" ["+std::to_string(*number)+"]";
    }
    return  std::nullopt;
}

std::optional<std::size_t> TypeDie::getSize() {
    Error error;
    Attr attr;
    auto res = dwarf_attr(die->die,DW_AT_byte_size,attr.ptr(),error.ptr());
    if (res!=DW_DLV_OK) {
        auto tag = die->getTag().value_or(0);
        if (tag==DW_TAG_pointer_type) return 4;
        if (tag==DW_TAG_const_type||tag==DW_TAG_volatile_type||tag==DW_TAG_typedef) {
            auto realType = die->getTypeDie();
            if (realType==std::nullopt) return std::nullopt;
            auto type = TypeDie(&*realType);
            return type.getSize();
        }
        if (tag==DW_TAG_array_type) {
            auto realType = die->getTypeDie();
            if (realType==std::nullopt) return std::nullopt;
            auto type = TypeDie(&*realType);
            auto typeSize = type.getSize();
            if (typeSize==std::nullopt)return std::nullopt;
            auto len = die->getArrayLen();
            if (len==std::nullopt)return std::nullopt;
            return *len**typeSize;
        }
        return std::nullopt;
    }
    Dwarf_Unsigned size;
    res = dwarf_formudata(*attr.ptr(),&size,error.ptr());
    if (res!=DW_DLV_OK)return std::nullopt;
    return size;
}

}
