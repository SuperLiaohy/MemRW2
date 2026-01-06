//
// Created by liaohy on 1/5/26.
//

#pragma once

#include "DisplayPluginInterface.hpp"
#include "VariTree.hpp"

class DisplayPluginInterface;
class VariComponent {
public:
    VariComponent(const std::shared_ptr<VariNode>& info);

    [[nodiscard]] const std::string& getName() const {return name;}
    [[nodiscard]] std::size_t getSize() const {return size;}
    [[nodiscard]] const std::string& getType() const {return type;}
    [[nodiscard]] std::size_t getAddress() const {return address;}

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
        float f64;
    } value;
private:
    bool active;
    std::string name;
    std::size_t size;
    std::string type;
    std::size_t address;
};
