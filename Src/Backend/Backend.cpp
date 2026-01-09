//
// Created by liaohy on 12/29/25.
//

#include "Backend.hpp"

#include "ExChartView.hpp"
#include "Src/USBInterface/DAPReader.h"

void Backend::init() {
    samplingWorker = std::jthread([this](const std::stop_token& stoken) {
        bool started = false;
        clock.reset();
        std::size_t fps = 0;
        std::size_t everyFrame = 0;
        while (!stoken.stop_requested()) {
            requestHandler();

            if (!running) {
                started = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            } else if (started == false) {
                daplink->resetMap(pluginContainer);
                clearPluginData();
                clock.reset();
                started = true;
            }

            auto runTime = clock.runTime()/1000.0;
            daplink->updateVari(pluginContainer);
            ++fps;
            if (runTime-everyFrame>1000) {
                everyFrame=runTime;
                std::cout << "fps: " <<fps<<std::endl;
                fps=0;
            }


            updatePlugin(runTime);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}

bool Backend::connect() {
    if (daplink->auto_connect()) {

        daplink->attach_to_target();
        daplink->auto_configure_ap();

        connected = true;
        emit connectedChanged();
    }
    return connected;
}

void Backend::disconnect() {
    if (running==true) {
        sync.sendRequest([this](){setRunning(false);});
    }
    daplink->disconnect();
    connected = false;
    emit connectedChanged();
}

void Backend::requestHandler() {
    sync.tryGetRequest([this]() {
        qDebug() << "handle request";
    });
}

void Backend::updatePlugin(qreal runTime) {
    for (auto& plugin: pluginContainer) {
        plugin->updateData(runTime);
    }
}

void Backend::clearPluginData() {
    for (auto& plugin: pluginContainer) {
        plugin->clearData();
    }
}

void Backend::pushPlugin(DisplayPluginInterface *plugin) {
    pluginContainer.push_back(plugin);
}

void Backend::erasePlugin(DisplayPluginInterface *plugin) {
    std::erase_if(pluginContainer, [plugin](void *ptr)->bool{return ptr==plugin;});
}
