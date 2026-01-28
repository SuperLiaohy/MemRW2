//
// Created by liaohy on 8/12/25.
//

#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <format>
#include <algorithm>
#include <memory>

constexpr uint32_t DAP_INTERFACE_CLASS = 0xFF;
constexpr uint32_t DAP_INTERFACE_SUBCLASS = 0x00;
constexpr uint32_t DAP_INTERFACE_PROTOCOL = 0x00;

struct USBInterface {
    std::string interface_name;
    int settingNum;

    uint8_t  bInterfaceClass;
    uint8_t  bInterfaceSubClass;
    uint8_t  bInterfaceProtocol;

    std::vector<uint8_t> bulk_in_endpoints;
    std::vector<uint8_t> bulk_out_endpoints;
};

struct USBDevDesc {
    uint16_t vendor_id;
    uint16_t product_id;
    std::string product_string;
    std::vector<USBInterface> interfaces;

    bool is_daplink();

    template<typename T>
    friend T& operator<<(T& ostream, const USBDevDesc& dev);
};

struct DapLinkDesc {
    uint16_t vendor_id;
    uint16_t product_id;
    std::string product_string;

    uint16_t interface_num;
    std::string interface_name;

    uint8_t bulk_in_endpoints;
    uint8_t bulk_out_endpoints;
    std::optional<uint8_t> bulk_in_swo_endpoints;

    DapLinkDesc(const std::shared_ptr<USBDevDesc>& dev) {
        this->vendor_id = dev->vendor_id;
        this->product_id = dev->product_id;
        this->product_string = dev->product_string;
        bool result = false;
        for (int i = 0; i < dev->interfaces.size(); ++i) {
            std::string trans_interface_name = dev->interfaces[i].interface_name;
            std::ranges::transform(trans_interface_name, trans_interface_name.begin(), tolower);
            result = (trans_interface_name.find("cmsis-dap") != std::string::npos);
            if (result && !dev->interfaces[i].bulk_in_endpoints.empty() && !dev->interfaces[i].bulk_out_endpoints.empty()) {
                if (dev->interfaces[i].bInterfaceClass == DAP_INTERFACE_CLASS && dev->interfaces[i].bInterfaceSubClass == DAP_INTERFACE_SUBCLASS && dev->interfaces[i].bInterfaceProtocol==DAP_INTERFACE_PROTOCOL) {
                    this->interface_num = i;
                    this->interface_name = dev->interfaces[i].interface_name;
                    this->bulk_in_endpoints = dev->interfaces[i].bulk_in_endpoints[0];
                    if (dev->interfaces[i].bulk_in_endpoints.size()>1)
                        this->bulk_in_swo_endpoints = dev->interfaces[i].bulk_in_endpoints[1];
                    this->bulk_out_endpoints = dev->interfaces[i].bulk_out_endpoints[0];
                    return;
                }
            }
        }
        throw std::runtime_error("DAP Bulk interface not found");
    };

    template<typename T>
    friend T& operator<<(T& ostream, const DapLinkDesc& dev);
};

std::shared_ptr<DapLinkDesc> findDapLinkDesc(const std::vector<std::shared_ptr<USBDevDesc>>& devs);

template<typename T>
T& operator<<(T& ostream, const USBDevDesc &dev) {
    ostream << "----\n";
    ostream << "product_string: " << dev.product_string << "\n";
    ostream << "vendor_id: " << std::format("0x{:x}", dev.vendor_id) << "\n";
    ostream << "product_id: " << std::format("0x{:x}", dev.product_id) << "\n";
    for (auto &interface : dev.interfaces) {
        ostream << "interface_name: " << interface.interface_name << "\n";
        ostream << "interface_settingNum: " << interface.settingNum << "\n";
        ostream << "bInterfaceClass: " << static_cast<uint32_t>(interface.bInterfaceClass) << "\n";
        ostream << "bInterfaceSubClass: " << static_cast<uint32_t>(interface.bInterfaceSubClass) << "\n";
        ostream << "bInterfaceProtocol: " << static_cast<uint32_t>(interface.bInterfaceProtocol) << "\n";
        for (int i = 0; i < interface.bulk_in_endpoints.size(); ++i) {
            ostream << "bulk_in_endpoints[" << i << "]: " << static_cast<uint32_t>(interface.bulk_in_endpoints[i]) << "; ";
        }
        if (!interface.bulk_in_endpoints.empty()) ostream << "\n";
        for (int i = 0; i < interface.bulk_out_endpoints.size(); ++i) {
            ostream << "bulk_out_endpoints[" << i << "]: " << static_cast<uint32_t>(interface.bulk_out_endpoints[i]) << "; ";
        }
        if (!interface.bulk_out_endpoints.empty()) ostream << "\n";
    }
    ostream << "----\n";
    return ostream;
}

template<typename T>
T& operator<<(T& ostream, const DapLinkDesc &dev) {
    ostream << "product_string: " << dev.product_string << "\n";
    ostream << "vendor_id: " << std::format("0x{:x}", dev.vendor_id) << "\n";
    ostream << "product_id: " << std::format("0x{:x}", dev.product_id) << "\n";
    ostream << "interface_num: " << dev.interface_num << "\n";
    ostream << "interface_name: " << dev.interface_name << "\n";
    ostream << "bulk_in_endpoints: " << static_cast<uint32_t>(dev.bulk_in_endpoints) << "\n";
    ostream << "bulk_out_endpoints: " << static_cast<uint32_t>(dev.bulk_out_endpoints) << "\n";
    if (dev.bulk_in_swo_endpoints.has_value()) {
        ostream << "bulk_in_swo_endpoints: " << static_cast<uint32_t>(*dev.bulk_in_swo_endpoints) << "\n";
    } else {
        ostream << "bulk_in_swo_endpoints: none\n";
    }
    return ostream;
}

bool is_usb_bulk(const std::string &str);
