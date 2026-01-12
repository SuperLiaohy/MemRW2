//
// Created by liaohy on 8/15/25.
//

#pragma once

#include <map>

#include "SWReg.h"
#include "USBBulk.h"


class DisplayPluginInterface;
class VariComponent;

struct DPRegs {
    std::optional<SW::DP::IDCODEReg> idcode;
    std::optional<SW::DP::ABORTReg> abort;
    std::optional<SW::DP::CTRL_STATReg> cs;
    std::optional<SW::DP::WCRReg> wcr;
    std::optional<SW::DP::RESENDReg> resend;
    std::optional<SW::DP::SELECTReg> select;
    std::optional<SW::DP::RDBUFFReg> rdbuff;
};

struct APRegs {
    std::optional<SW::MEM_AP::CSWReg> csw;
    std::optional<SW::MEM_AP::TARReg> tar;
    std::optional<SW::MEM_AP::DRWReg> drw;
    std::optional<SW::MEM_AP::BDReg> bd0;
    std::optional<SW::MEM_AP::BDReg> bd1;
    std::optional<SW::MEM_AP::BDReg> bd2;
    std::optional<SW::MEM_AP::BDReg> bd3;
    std::optional<SW::MEM_AP::MBTReg> mbt;
    std::optional<SW::MEM_AP::T0TRReg> t0tr;
    std::optional<SW::MEM_AP::CFG1Reg> cfg1;
    std::optional<SW::MEM_AP::BASE_LARGEReg> base_large;
    std::optional<SW::MEM_AP::CFGReg> cfg;
    std::optional<SW::MEM_AP::BASEReg> base;
    std::optional<SW::MEM_AP::IDRReg> idr;
};

struct SerialDebugInterface {
    DPRegs dp;
    APRegs ap;
};

union all_form {
    int8_t i8[4];
    uint8_t u8[4];
    int16_t i16[2];
    uint16_t u16[2];
    int32_t i32;
    uint32_t u32;
    float f32;
};

namespace DAP {
enum Port {
    Default_Port = 0,
    SWD_Port = 1,
    JTAG_Port = 2,
};

enum ResponseStatus {
    DAP_OK = 0x00,
    DAP_ERROR = 0xFF
};
enum TransferResponseStatus {
    TRANSFER_OK = 0x01,
    TRANSFER_WAIT = 0x02,
    TRANSFER_FAULT = 0x04,
    TRANSFER_NO_ACK = 0x07,
};
#pragma pack(push,1)
struct TransferRequest {
    struct {
        uint8_t APnDP: 1;
        uint8_t RnW: 1;
        uint8_t reg: 2;
        uint8_t ValueMatch: 1;
        uint8_t MatchMask: 1;
        uint8_t RES: 1;
        uint8_t TimeStamp: 1;
    } request;

    uint32_t data;
};

struct TransferResponse {
    uint32_t TimeStamp;
    all_form bit_data;
};
#pragma pack(pop)
}

class DAPReader {
public:
    DAPReader();
    ~DAPReader();

    bool auto_connect();
    void disconnect();

    void attach_to_target();
    void auto_configure_ap();
    int dap_connect(uint8_t port);

    uint32_t read_mem32(uint32_t addr);

    void write_mem8(uint32_t addr, uint8_t data);
    void write_mem16(uint32_t addr, uint16_t data);
    void write_mem32(uint32_t addr, uint32_t data);
    void write_mem64(uint32_t addr, uint64_t data);

    int set_swj_clock(uint32_t clock);
    int set_swj_sequence(const uint8_t *sequence, uint8_t len);
    int set_swj_sequence(const std::vector<uint8_t> &sequence) {return set_swj_sequence(sequence.data(), sequence.size());}

    int transfer(const DAP::TransferRequest &requests, DAP::TransferResponse &responses);
    int transfer(const std::vector<DAP::TransferRequest> &requests, std::vector<DAP::TransferResponse> &responses);

    static DAP::TransferRequest Request(uint8_t APnDP, uint8_t RnW, uint8_t reg, uint8_t ValueMatch, uint8_t MatchMask,uint8_t TimeStamp, uint32_t data);
    static DAP::TransferRequest
    APWriteRequest(uint8_t reg, uint32_t data, uint8_t TimeStamp = 0, uint8_t MatchMask = 0) {
        return Request(1, 0, reg, 0, MatchMask, TimeStamp, data);
    }
    static DAP::TransferRequest APReadRequest(uint8_t reg, uint32_t data = 0, uint8_t TimeStamp = 0,uint8_t ValueMatch = 0) {
        return Request(1, 1, reg, ValueMatch, 0, TimeStamp, data);
    }
    static DAP::TransferRequest
    DPWriteRequest(uint8_t reg, uint32_t data, uint8_t TimeStamp = 0, uint8_t MatchMask = 0) {
        return Request(0, 0, reg, 0, MatchMask, TimeStamp, data);
    }
    static DAP::TransferRequest DPReadRequest(uint8_t reg, uint32_t data = 0, uint8_t TimeStamp = 0,uint8_t ValueMatch = 0) {
        return Request(0, 1, reg, ValueMatch, 0, TimeStamp, data);
    }

    bool writeValueToVari(VariComponent* vari,double value);

    void resetMap(const std::vector<DisplayPluginInterface*>& plugins);
    int updateVari(std::vector<DisplayPluginInterface*>& plugins);
private:
    void getData(VariComponent* vari,all_form* words);
    void pushRequest(const DAP::TransferRequest& request);
    void pushMapRequests(std::size_t addr);
    void generateMapRequests();
    int transferFromMapRequests();

    static SerialDebugInterface sw;
    std::unique_ptr<USBBulk> usb;
    std::vector<uint8_t> response_buffer;

    std::multimap<std::size_t,VariComponent*> addrMap;
    std::vector<std::uint8_t> mapRequestBuf;
    std::size_t requestCount;
};
