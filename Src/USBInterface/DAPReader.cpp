//
// Created by liaohy on 8/15/25.
//

#include "DAPReader.h"

#include <bitset>
#include <cstring>
#include <iostream>
#include <thread>

#include "DisplayPluginInterface.hpp"
#include "VariComponent.hpp"

SerialDebugInterface DAPReader::sw;

namespace {
    uint32_t DAP_INTERFACE_CLASS = 0xFF;
    uint32_t DAP_INTERFACE_SUBCLASS = 0x00;
    uint32_t DAP_INTERFACE_PROTOCOL = 0x00;


    uint8_t DAP_SWJ_Clock = 0x11;
    uint8_t DAP_SWJ_Sequence = 0x12;

    uint8_t DAP_Transfer = 0x05;
    uint8_t DAP_TransferBlock = 0x06;

    uint8_t DAP_Connect = 0x02;
}

DAPReader::DAPReader() : response_buffer(255 * 5 + 3), mapRequestBuf(255*5+3),requestCount(0) {
    // auto_connect();
}

DAPReader::~DAPReader() {
    qDebug()<<"fuck dap";
    disconnect();
}

bool DAPReader::auto_connect() {
    USBBulk::init();
    libusb_device **device_list = nullptr;
    int32_t device_count = libusb_get_device_list(USBBulk::Context(), &device_list);
    if (device_count < 0) {
        std::cout << "Could not find any usb devices."<<std::endl;
        return false;
    }
    std::vector<std::shared_ptr<USBDevDesc> > devs;
    for (ssize_t i = 0; i < device_count; ++i) {
        libusb_device *device = device_list[i];
        libusb_device_descriptor desc{};
        auto item = std::make_shared<USBDevDesc>();

        // 获取设备描述符
        int result = libusb_get_device_descriptor(device, &desc);
        if (result != LIBUSB_SUCCESS)
            continue;

        // 打开设备以获取字符串描述符
        libusb_device_handle *handle = nullptr;
        result = libusb_open(device, &handle);
        if (result != LIBUSB_SUCCESS || handle == nullptr)
            continue;

        std::string product_string = "N/A";
        // 获取product string (iProduct字段)
        if (desc.iProduct) {
            char string_buffer[256];
            result = libusb_get_string_descriptor_ascii(handle, desc.iProduct,
                                                        reinterpret_cast<unsigned char *>(string_buffer),
                                                        sizeof(string_buffer));
            if (result > 0) {
                product_string = std::string(string_buffer);
            }
        }
        libusb_close(handle);

        item->vendor_id = desc.idVendor;
        item->product_id = desc.idProduct;
        item->product_string = product_string;
        if (item->is_daplink())
            devs.push_back(item);
    }
    libusb_free_device_list(device_list, device_count);

    bool found = false;
    for (auto &usb_desc: devs) {
        libusb_device_handle *handle = nullptr;
        libusb_config_descriptor *config_descriptor = nullptr;
        handle = libusb_open_device_with_vid_pid(USBBulk::Context(), usb_desc->vendor_id, usb_desc->product_id);
        if (!handle) continue;
        int res = libusb_get_active_config_descriptor(libusb_get_device(handle), &config_descriptor);
        if (res != 0) continue;
        for (int i = 0; i < config_descriptor->bNumInterfaces; ++i) {
            const libusb_interface &interface = config_descriptor->interface[i];
            // default use index 0
            if (interface.num_altsetting > 0) {
                const libusb_interface_descriptor *desc = interface.altsetting;
                if (desc->iInterface > 0) {
                    usb_desc->interface_name = "N/A";
                    char name_data[256];
                    if (libusb_get_string_descriptor_ascii(handle, desc->iInterface,
                                                           reinterpret_cast<unsigned char *>(name_data),
                                                           sizeof(name_data)))
                        usb_desc->interface_name = name_data;
                    if (desc->bInterfaceClass == DAP_INTERFACE_CLASS &&
                        desc->bInterfaceSubClass == DAP_INTERFACE_SUBCLASS &&
                        desc->bInterfaceProtocol == DAP_INTERFACE_PROTOCOL) {
                        usb_desc->interface_num = desc->bInterfaceNumber;
                        bool bulkinHostPC = true;
                        for (int j = 0; j < desc->bNumEndpoints; ++j) {
                            if ((desc->endpoint[j].bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN && bulkinHostPC){
                                usb_desc->bulk_in_endpoints = desc->endpoint[j].bEndpointAddress;
                                bulkinHostPC = false;
                            }
                            else usb_desc->bulk_out_endpoints = desc->endpoint[j].bEndpointAddress;
                        }
                        found = true;
                        break;
                    }
                }
            }
        }
        libusb_close(handle);
        libusb_free_config_descriptor(config_descriptor);
        if (found == true) {
            usb = std::make_unique<USBBulk>(usb_desc);
            break;
        };
    }
    if (usb == nullptr) {
        std::cout << "no usb bulk be found." << std::endl;
        return false;
    }
    std::cout << usb->desc << std::endl;
    return true;
}

void DAPReader::disconnect() {
    usb.reset();
    USBBulk::exit();
}

void DAPReader::attach_to_target() {
    this->dap_connect(DAP::SWD_Port);
    this->set_swj_clock(10'000'000);
    this->set_swj_sequence({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
    this->set_swj_sequence({0x9E, 0xE7});
    this->set_swj_sequence({0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF});
    this->set_swj_sequence({0x00});
    DAP::TransferResponse response{};
    this->transfer(DPReadRequest(SW::DP::IDCODE), response);
    std::cout << "DPIDR: " << std::hex << response.bit_data.u32 << std::dec << std::endl;
    auto idr = std::bit_cast<SW::DP::IDCODEReg>(response.bit_data);
    std::cout << idr << std::endl;
    SW::DP::ABORTReg abort{};
    abort.DAPABORT = 0;
    abort.STKCMPCLR = 1;
    abort.STKERRCLR = 1;
    abort.WDERRCLR = 1;
    abort.ORUNERRCLR = 1;
    this->transfer(DPWriteRequest(SW::DP::ABORT, std::bit_cast<uint32_t>(abort)), response);
    SW::DP::CTRL_STATReg cs{};
    cs.CSYSPWRUPREQ = 1;
    cs.CDBGPWRUPREQ = 1;
    this->transfer(DPWriteRequest(SW::DP::CTRL_STAT, std::bit_cast<uint32_t>(cs)), response);
    response.bit_data.u32 = 0;
    for (int i = 0; i < 10; ++i) {
        this->transfer(DPReadRequest(SW::DP::CTRL_STAT), response);
        auto cs_feedback = std::bit_cast<SW::DP::CTRL_STATReg>(response.bit_data.u32);
        if (cs_feedback.CDBGPWRUPACK == 1 && cs_feedback.CSYSPWRUPACK == 1) {
            std::cout << "Debug interface domain is powered up." << std::endl;
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::cout << "failed to power up debug interface domain." << std::endl;
}

uint32_t DAPReader::read_mem32(uint32_t addr) {
    DAP::TransferResponse response{};
    std::vector<DAP::TransferResponse> responses(2);
    this->transfer({APWriteRequest(SW::MEM_AP::TAR, addr), APReadRequest(SW::MEM_AP::DRW)}, responses);
    return responses[1].bit_data.u32;
}

void DAPReader::write_mem8(uint32_t addr, uint8_t data) {

}

void DAPReader::write_mem16(uint32_t addr, uint16_t data) {
}

void DAPReader::write_mem32(uint32_t addr, uint32_t data) {
}

void DAPReader::write_mem64(uint32_t addr, uint64_t data) {
}

void DAPReader::auto_configure_ap() {
    sw.dp.select = std::make_optional<SW::DP::SELECTReg>();
    uint8_t APSEL = 0;
    bool found_MEM_AP = false;
    while (!found_MEM_AP) {
        DAP::TransferResponse response{};
        sw.dp.select->CTRLSEL = 0;
        sw.dp.select->APSEL = APSEL;
        sw.dp.select->APBANKSEL = 0xF;
        this->transfer(DPWriteRequest(SW::DP::SELECT, std::bit_cast<uint32_t>(sw.dp.select.value())), response);
        std::cout << "--------------APSEL:" << static_cast<uint32_t>(APSEL) << "--------------" << std::endl;
        this->transfer(APReadRequest(SW::MEM_AP::IDR), response);
        sw.ap.idr = std::bit_cast<SW::MEM_AP::IDRReg>(response.bit_data.u32);
        std::cout << sw.ap.idr.value() << std::endl;
        if (sw.ap.idr->CLASS == 0b1000) found_MEM_AP = true;
        sw.dp.select->APBANKSEL = 0x0;
        this->transfer(DPWriteRequest(SW::DP::SELECT, std::bit_cast<uint32_t>(sw.dp.select.value())), response);
        this->transfer(APReadRequest(SW::MEM_AP::CSW), response);
        sw.ap.csw = std::bit_cast<SW::MEM_AP::CSWReg>(response.bit_data);
        std::cout << sw.ap.csw.value() << std::endl;

        sw.ap.csw->AddrInc = 0b01;
        this->transfer(APWriteRequest(SW::MEM_AP::CSW,std::bit_cast<uint32_t>(sw.ap.csw.value())), response);

        this->transfer(APReadRequest(SW::MEM_AP::CSW), response);
        sw.ap.csw = std::bit_cast<SW::MEM_AP::CSWReg>(response.bit_data);
        std::cout << sw.ap.csw.value() << std::endl;

        // csw.DbgSwEnable = 1;
        // csw.AddrInc = 1;
        // this->transfer(APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(csw)), response);
        // this->transfer(APReadRequest(SW::MEM_AP::CSW), response);
        // csw = std::bit_cast<SW::MEM_AP::CSWReg>(response.data);
        // std::cout << csw << std::endl;
        ++APSEL;
        if (APSEL>100)
            break;
    }
}

int DAPReader::dap_connect(uint8_t port) {
    std::vector<uint8_t> request{DAP_Connect, port};
    usb->transmit(request);
    std::vector<uint8_t> response_buffer(256);
    usb->receive(response_buffer);
    if (response_buffer[0] != DAP_Connect) return DAP::DAP_ERROR;
    return response_buffer[1];
}

int DAPReader::set_swj_clock(uint32_t clock) {
    std::vector<uint8_t> data = {DAP_SWJ_Clock, 0, 0, 0, 0};
    *reinterpret_cast<uint32_t *>(&data[1]) = clock;
    usb->transmit(data);
    std::vector<uint8_t> response(256);
    usb->receive(response);
    if (response[0] != DAP_SWJ_Clock) return DAP::DAP_ERROR;
    return response[1];
}

int DAPReader::set_swj_sequence(const uint8_t *sequence, uint8_t len) {
    std::vector<uint8_t> data(len + 2);
    data[0] = DAP_SWJ_Sequence;
    data[1] = len * 8;
    for (int i = 0; i < len; ++i) {
        data[i + 2] = sequence[i];
    }
    data.resize(len + 2);
    usb->transmit(data.data(), data.size());
    std::vector<uint8_t> response(256);
    usb->receive(response);
    if (response[0] != DAP_SWJ_Sequence) return DAP::DAP_ERROR;
    return response[1];
}

int DAPReader::transfer(const DAP::TransferRequest &requests, DAP::TransferResponse &responses) {
    std::vector<uint8_t> request_buffer(3 + 5);
    request_buffer[0] = DAP_Transfer;
    request_buffer[1] = 0;
    request_buffer[2] = 1;
    uint32_t index = 0;
    if (requests.request.RnW == 1 && requests.request.ValueMatch == 0) {
        request_buffer[3] = std::bit_cast<uint8_t>(requests.request);
        index = 1;
    } else {
        memcpy(&request_buffer[3], &requests, 5);
        index = 5;
    }
    request_buffer.resize(3 + index);
    usb->transmit(request_buffer);

    response_buffer.resize(response_buffer.capacity());
    usb->receive(response_buffer);
    if (response_buffer[0] != DAP_Transfer) return DAP::DAP_ERROR;
    uint8_t TransferCount = response_buffer[1];
    uint32_t *word = reinterpret_cast<uint32_t *>(&response_buffer[3]);
    index = 0;
    if (requests.request.TimeStamp) {
        responses.TimeStamp = word[index++];
    }
    if (requests.request.RnW == 1 && requests.request.ValueMatch == 0)
        responses.bit_data.u32 = word[index++];
    return response_buffer[2];
}

int DAPReader::transfer(const std::vector<DAP::TransferRequest> &requests,
                        std::vector<DAP::TransferResponse> &responses) {
    std::vector<uint8_t> request_buffer(3 + requests.size() * 5);
    request_buffer[0] = DAP_Transfer;
    request_buffer[1] = 0;
    request_buffer[2] = requests.size();
    // memcpy(&request_buffer[3],requests.data(),requests.size()*5);
    uint32_t index = 0;
    for (int i = 0; i < requests.size(); ++i) {
        if (requests[i].request.RnW == 1 && requests[i].request.ValueMatch == 0) {
            request_buffer[3 + index] = std::bit_cast<uint8_t>(requests[i].request);
            ++index;
        } else {
            memcpy(&request_buffer[3 + index], &requests[i], 5);
            index += 5;
        }
    }
    request_buffer.resize(3 + index);
    response_buffer.resize(response_buffer.capacity());

    usb->transmit(request_buffer);
    usb->receive(response_buffer);

    if (response_buffer[0] != DAP_Transfer) return DAP::DAP_ERROR;
    uint8_t TransferCount = response_buffer[1];
    uint32_t *word = reinterpret_cast<uint32_t *>(&response_buffer[3]);
    index = 0;
    for (int i = 0; i < TransferCount; ++i) {
        if (requests[i].request.TimeStamp) {
            responses[i].TimeStamp = word[index++];
        }
        if (requests[i].request.RnW == 1 && requests[i].request.ValueMatch == 0)
            responses[i].bit_data.u32 = word[index++];
    }
    return response_buffer[2];
}

DAP::TransferRequest DAPReader::Request(uint8_t APnDP, uint8_t RnW, uint8_t reg, uint8_t ValueMatch, uint8_t MatchMask,
                                        uint8_t TimeStamp, uint32_t data) {
    DAP::TransferRequest request{};
    // request.request = APnDP | (RnW<<1) | (reg&0b0000'1100) | (ValueMatch<<4) | (MatchMask<<5) | (TimeStamp<<6);
    request.request.APnDP = APnDP;
    request.request.RnW = RnW;
    request.request.reg = (reg & 0b0000'1100) >> 2;
    request.request.ValueMatch = ValueMatch;
    request.request.MatchMask = MatchMask;
    request.request.TimeStamp = TimeStamp;
    request.data = data;
    return request;
}

void DAPReader::resetMap(const std::vector<DisplayPluginInterface *> &plugins) {
    addrMap.clear();
    for (auto plugin: plugins) {
        for (auto & variComponent: plugin->variContainer) {
            auto base = variComponent->getAddress()&0xfffffffC;
            addrMap.insert({base,variComponent.get()});
            if (((variComponent->getAddress()&0x00000003)+variComponent->getSize())>4) {
                addrMap.insert({base+4,nullptr});
                if (((variComponent->getAddress()&0x00000003)+variComponent->getSize())>8)
                    addrMap.insert({base+8,nullptr});
            }
        }
    }
    generateMapRequests();
}

int DAPReader::transferFromMapRequests() {
    usb->transmit(mapRequestBuf);
    usb->receive(response_buffer);
    if (response_buffer.size()<3) return DAP::DAP_ERROR;
    if (response_buffer[0] != DAP_Transfer) return DAP::DAP_ERROR;
    if (response_buffer[2] != DAP::TRANSFER_OK) return response_buffer[2];
    if (response_buffer[1]!=(requestCount)) return DAP::DAP_ERROR;
    return DAP::TRANSFER_OK;
}

int DAPReader::updateVari(std::vector<DisplayPluginInterface *> &plugins) {
    auto res = transferFromMapRequests();
    if (res!=DAP::TRANSFER_OK)return res;
    auto word = reinterpret_cast<all_form *>(&response_buffer[3]);
    std::size_t last = 0;
    std::size_t index = -1;
    for (auto item: addrMap) {
        auto base = item.first;
        auto vari = item.second;
        if (vari==nullptr)continue;
        if (last!=base) {++index;}
        getData(vari,&word[index]);
        last = base;
    }
    return DAP::TRANSFER_OK;
}

void DAPReader::pushRequest(const DAP::TransferRequest &request) {
    if (request.request.RnW == 1 && request.request.ValueMatch == 0 && request.request.MatchMask == 0) {
        mapRequestBuf.push_back(std::bit_cast<std::uint8_t>(request.request));
    } else {
        mapRequestBuf.resize(mapRequestBuf.size()+5);
        uint8_t*ptr=&mapRequestBuf[mapRequestBuf.size()-5];
        memcpy(ptr,&request,5);
    }
}

void DAPReader::pushMapRequests(std::size_t addr) {
    if (sw.ap.tar.has_value()) {
        if (sw.ap.tar->data != addr) {
            ++requestCount;
            pushRequest(APWriteRequest(SW::MEM_AP::TAR, addr));
        }
        sw.ap.tar->data = addr+4;
        if ((sw.ap.tar->data)&(0x400-1)) {
            sw.ap.tar.reset();
        }
    } else {
        ++requestCount;
        pushRequest(APWriteRequest(SW::MEM_AP::TAR, addr));
        sw.ap.tar = SW::MEM_AP::TARReg(addr+4);
        if ((sw.ap.tar->data)&(0x400-1)) {
            sw.ap.tar.reset();
        }
    }
    ++requestCount;
    pushRequest(APReadRequest(SW::MEM_AP::DRW));
}

void DAPReader::generateMapRequests() {
    requestCount=0;
    mapRequestBuf.clear();
    mapRequestBuf.push_back(5);
    mapRequestBuf.push_back(0);
    mapRequestBuf.push_back(0);
    std::size_t last = 0;
    for (auto & addr: addrMap) {
        auto& base = addr.first;
        if (last!=base) {
            pushMapRequests(base);
            last=base;
        }
    }
    mapRequestBuf[2]=requestCount;
}

void DAPReader::getData(VariComponent *vari, all_form *words) {
        auto offset = vari->getAddress()&0x03;
        switch (vari->getType()) {
            case VariComponent::Type::BOOL: {
                vari->fValue = vari->value.i8 = words[0].i8[offset];
            } break;
            case VariComponent::Type::INT8: {
                vari->fValue = vari->value.i8 = words[0].i8[offset];
            }
            break;
            case VariComponent::Type::UINT8: {
                vari->fValue = vari->value.u8 = words[0].u8[offset];;
            }
            break;
            case VariComponent::Type::INT16: {
                if (offset > 2)
                    vari->fValue = vari->value.i16 = std::bit_cast<int16_t>(
                        static_cast<uint16_t>((words[0].u8[offset] | words[1].u8[0] << 8)));
                else
                    vari->fValue = vari->value.i16 = std::bit_cast<int16_t>(
                        static_cast<uint16_t>((words[0].u8[offset] | words[0].u8[offset + 1] << 8)));
            }
            break;
            case VariComponent::Type::UINT16: {
                if (offset > 2)
                    vari->fValue = vari->value.u16 = (static_cast<uint16_t>((words[0].u8[offset] | words[1].u8[0] << 8)));
                else
                    vari->fValue = vari->value.u16 = (static_cast<uint16_t>((words[0].u8[offset] | words[0].u8[offset + 1] << 8)));
            }
            break;
            case VariComponent::Type::INT32: {
                switch (offset) {
                    case 0:
                        vari->fValue = vari->value.i32 = words[0].i32;
                        break;
                    case 1:
                        vari->fValue = vari->value.i32 = static_cast<int32_t>(words[0].u8[1]
                                                               | words[0].u8[2] << 8
                                                               | words[0].u8[3] << 16
                                                               | words[1].u8[0] << 24);
                        break;
                    case 2:
                        vari->fValue = vari->value.i32 = static_cast<int32_t>(words[0].u16[1]
                                                               | words[1].u16[0] << 16);
                        break;
                    case 3:
                        vari->fValue = vari->value.i32 = static_cast<int32_t>(words[0].u8[3]
                                                               | words[1].u8[0] << 8
                                                               | words[1].u8[1] << 16
                                                               | words[1].u8[2] << 24);
                        break;
                    default:
                        break;
                }
            }
            break;
            case VariComponent::Type::UINT32: {
                switch (offset) {
                    case 0:
                        vari->fValue = vari->value.u32 = words[0].u32;
                        break;
                    case 1:
                        vari->fValue = vari->value.u32 = (words[0].u8[1]
                                           | words[0].u8[2] << 8
                                           | words[0].u8[3] << 16
                                           | words[1].u8[0] << 24);
                        break;
                    case 2:
                        vari->fValue = vari->value.u32 = (words[0].u16[1]
                                           | words[1].u16[0] << 16);
                        break;
                    case 3:
                        vari->fValue = vari->value.u32 = (words[0].u8[3]
                                           | words[1].u8[0] << 8
                                           | words[1].u8[1] << 16
                                           | words[1].u8[2] << 24);
                        break;
                    default:
                        break;
                }
            }
            break;
            case VariComponent::Type::FLOAT: {
                switch (offset) {
                    case 0:
                        vari->fValue = vari->value.f32 = words[0].f32;
                        break;
                    case 1:
                        vari->fValue = vari->value.f32 = std::bit_cast<float>(words[0].u8[1]
                                                               | words[0].u8[2] << 8
                                                               | words[0].u8[3] << 16
                                                               | words[1].u8[0] << 24);
                        break;
                    case 2:
                        vari->fValue = vari->value.f32 = std::bit_cast<float>(words[0].u16[1]
                                                               | words[1].u16[0] << 16);
                        break;
                    case 3:
                        vari->fValue = vari->value.f32 = std::bit_cast<float>(words[0].u8[3]
                                                               | words[1].u8[0] << 8
                                                               | words[1].u8[1] << 16
                                                               | words[1].u8[2] << 24);
                        break;
                    default:
                        break;
                }
            }
            break;
            case VariComponent::Type::DOUBLE:
            case VariComponent::Type::INT64:
            case VariComponent::Type::UINT64:
                break;
        }
}

bool DAPReader::writeValueToVari(VariComponent *vari, double value) {
    auto offset = vari->getAddress()&0x03;
    auto base = vari->getAddress()-offset;
    std::vector<DAP::TransferRequest> reg_request;
    std::vector<DAP::TransferResponse> reg_responses(4);
    reg_request.reserve(4);
    volatile all_form data;
    data.u32 = 0;
    switch (vari->getType()) {
        case VariComponent::Type::BOOL: {
            DAPReader::sw.ap.csw->Size = 0b000;
            data.u8[offset] = static_cast<bool>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
            DAPReader::sw.ap.csw->Size = 0b010;
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
        }break;
        case VariComponent::Type::INT8: {
            DAPReader::sw.ap.csw->Size = 0b000;
            data.i8[offset] = static_cast<int8_t>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
            DAPReader::sw.ap.csw->Size = 0b010;
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
        }
            break;
        case VariComponent::Type::UINT8: {
            DAPReader::sw.ap.csw->Size = 0b000;
            data.u8[offset] = static_cast<uint8_t>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
            DAPReader::sw.ap.csw->Size = 0b010;
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
        }
            break;
        case VariComponent::Type::INT16: {
            if (offset==1||offset==3) return false;
            DAPReader::sw.ap.csw->Size = 0b001;
            data.i16[offset/2] = static_cast<int16_t>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
            DAPReader::sw.ap.csw->Size = 0b010;
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
        }
            break;
        case VariComponent::Type::UINT16:{
            if (offset==1||offset==3) return false;
            DAPReader::sw.ap.csw->Size = 0b001;
            data.i16[offset/2] = static_cast<uint16_t>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
            DAPReader::sw.ap.csw->Size = 0b010;
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::CSW, std::bit_cast<uint32_t>(*DAPReader::sw.ap.csw)));
        }
            break;
        case VariComponent::Type::INT32: {
            if (offset!=0) return false;
            data.i32 = static_cast<int32_t>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
        }
            break;
        case VariComponent::Type::UINT32: {
            if (offset!=0) return false;
            data.u32 = static_cast<uint32_t>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
        }break;
        case VariComponent::Type::INT64:
            return false;
            break;
        case VariComponent::Type::UINT64:
            return false;
            break;
        case VariComponent::Type::FLOAT:
            if (offset!=0) return false;
            data.f32 = static_cast<float>(value);
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, vari->getAddress()));
            reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data.u32));
            break;
        case VariComponent::Type::DOUBLE:
            return false;
            break;
    }
    if (transfer(reg_request,reg_responses)!=DAP::TRANSFER_OK) {
        qDebug()<<"transfer failed,write failed.";
        return false;
    }
    return true;
}

bool DAPReader::resetTarget() {
    constexpr uint32_t address = 0xE000E000UL+0x0D00UL+0x00C;
    std::vector<DAP::TransferRequest> reg_request;
    std::vector<DAP::TransferResponse> reg_responses(2);
    reg_request.reserve(4);

    std::vector<DAP::TransferResponse> responses(2);
    if  (this->transfer({APWriteRequest(SW::MEM_AP::TAR, address), APReadRequest(SW::MEM_AP::DRW)}, responses)!=DAP::TRANSFER_OK)
        return false;

    uint32_t origin = responses[0].bit_data.u32;
    uint32_t data = (uint32_t)((0x5FAUL << 16U)    |
                           (origin & (7UL << 8U)) |
                            (1UL << 2U)    );;

    reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::TAR, address));
    reg_request.push_back(DAPReader::APWriteRequest(SW::MEM_AP::DRW, data));
    if (transfer(reg_request,reg_responses)!=DAP::TRANSFER_OK) {
        qDebug()<<"transfer failed,write failed.";
        return false;
    }
    qDebug()<<"resetTarget";

    return true;
}

