//
// Created by liaohy on 1/5/26.
//

#pragma once

#include "DisplayPluginInterface.hpp"
#include "VariTree.hpp"

class DisplayPluginInterface;
class VariComponent {
public:
    enum class Type { INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64, FLOAT, DOUBLE};

    VariComponent(VariNode* info);

    [[nodiscard]] const std::string& getName() const {return name;}
    [[nodiscard]] std::size_t getSize() const {return size;}
    [[nodiscard]] Type getType() const {return type;}
    [[nodiscard]] std::size_t getAddress() const {return address;}

    qreal fValue;
    union {
        std::uint8_t u8;
        std::uint16_t u16;
        std::uint32_t u32;
        std::uint64_t u64;

        std::int8_t i8;
        std::int16_t i16;
        std::int32_t i32;
        std::int64_t i64;

        float f32;
        double f64;
    } value;
private:
    bool active;
    std::string name;
    std::size_t size;
    Type type;
    std::size_t address;
};
