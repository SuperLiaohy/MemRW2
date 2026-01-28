//
// Created by liaohy on 8/12/25.
//

#include "USBDevDesc.h"
#include <algorithm>
#include <iostream>

bool USBDevDesc::is_daplink() {
    bool result = false;
    std::string trans_name = this->product_string;
    std::ranges::transform(trans_name, trans_name.begin(), tolower);
    result = (trans_name.find("cmsis-dap") != std::string::npos);

    for (const auto& interface: this->interfaces) {
        if (result) break;
        std::string trans_interface_name = interface.interface_name;
        std::ranges::transform(trans_interface_name, trans_interface_name.begin(), tolower);
        result = (trans_interface_name.find("cmsis-dap") != std::string::npos);
    }

    return result;
};


std::shared_ptr<DapLinkDesc> findDapLinkDesc(const std::vector<std::shared_ptr<USBDevDesc>>& devs) {
    for (auto& dev : devs) {
        if (dev->is_daplink()) {
            return std::make_shared<DapLinkDesc>(dev);
        }
    }
    return nullptr;
}

bool is_usb_bulk(const std::string &str) {
    std::string trans_name = str;
    std::ranges::transform(trans_name,trans_name.begin(), tolower);
    return (trans_name.find("cmsis-dap") != std::string::npos);
}

