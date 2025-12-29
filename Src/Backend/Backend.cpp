//
// Created by liaohy on 12/29/25.
//

#include "Backend.hpp"

void Backend::init() {
    samplingWorker = std::jthread([this](const std::stop_token& stoken) {
        while (!stoken.stop_requested()) {
            if (!running) std::this_thread::sleep_for(std::chrono::milliseconds(100));



            sync.tryGetRequest([this]() {
                qDebug()<<"remove line";
            });
        }
    });
}
