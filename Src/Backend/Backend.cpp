//
// Created by liaohy on 12/29/25.
//

#include "Backend.hpp"

#include "ExChartView.hpp"

void Backend::init() {
    samplingWorker = std::jthread([this](const std::stop_token& stoken) {
        bool lastRun = running;
        clock.reset();
        while (!stoken.stop_requested()) {
            requestHandler();
            if (lastRun==false&&running==true) {
                clock.reset();
            }
            lastRun = running;
            if (!running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }

            auto runTime = clock.runTime()/1000.0;

            updatePlugin(runTime);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
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

void Backend::pushPlugin(DisplayPluginInterface *plugin) {
    pluginContainer.push_back(plugin);
}

void Backend::erasePlugin(DisplayPluginInterface *plugin) {
    std::erase_if(pluginContainer, [plugin](void *ptr)->bool{return ptr==plugin;});
}
