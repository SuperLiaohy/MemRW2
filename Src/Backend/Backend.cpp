//
// Created by liaohy on 12/29/25.
//

#include "Backend.hpp"

#include <qqmlcontext.h>

#include "ExChartView.hpp"
#include "Src/USBInterface/DAPReader.h"

void Backend::init() {
    samplingWorker = std::jthread([this](const std::stop_token& stoken) {
        bool started = false;
        clock.reset();
        std::size_t Hz = 0;
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
                everyFrame = 0;
                Hz = 0;
                started = true;
            }

            auto runTime = clock.runTime()/1000.0;
            daplink->updateVari(pluginContainer);
            ++Hz;
            if (runTime-everyFrame>1000) {
                everyFrame=runTime;
                samplingHz = Hz;
                emit samplingHzChanged();
                std::cout << "Hz: " <<Hz<<std::endl;
                Hz=0;
            }


            updatePlugin(runTime);
            if (delayUs!=0) {
                std::this_thread::sleep_for(std::chrono::microseconds(delayUs));
            }
        }
    });
}

void Backend::registerVariModel(const QQmlApplicationEngine &engine) {
    engine.rootContext()->setContextProperty("myTreeModel", variModel.get());
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

QStringList Backend::reloadVari() {
    QStringList variList;
    for (auto plugin: pluginContainer) {
        variList.append(plugin->reloadVari());
    }
    return variList;
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
