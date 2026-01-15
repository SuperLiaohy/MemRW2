//
// Created by liaohy on 12/29/25.
//

#include "Backend.hpp"

#include <qqmlcontext.h>

#include "ExChartView.hpp"
#include "Src/USBInterface/DAPReader.h"

void Backend::init() {
    samplingWorker = std::jthread([this](std::stop_token stoken) {
        bool started = false;
        int res;
        std::size_t errorCount = 0;
        clock.reset();
        std::size_t Hz = 0;
        std::size_t everyFrame = 0;
        while (!stoken.stop_requested()) {
            requestHandler();

            if (!running) {
                if (started) pluginsEnd();
                started = false;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            } else if (started == false) {
                daplink->resetMap(pluginContainer);
                pluginsStart();
                clock.reset();
                everyFrame = 0;
                Hz = 0;
                started = true;
            }

            auto runTime = clock.runTime()/1000.0;
            if(daplink->updateVari(pluginContainer)!=DAP::TRANSFER_OK) {
                if(++errorCount>200) {
                    running.store(false);
                    QMetaObject::invokeMethod(this, "runningChanged", Qt::QueuedConnection);
                    QMetaObject::invokeMethod(this, "linkErrorHappen", Qt::QueuedConnection);
                    errorCount = 0;
                    continue;
                }
            } else {errorCount/=2;}

            ++Hz;
            if (runTime-everyFrame>1000) {
                everyFrame=runTime;
                samplingHz = Hz;
                QMetaObject::invokeMethod(this, "samplingHzChanged", Qt::QueuedConnection);
                std::cout << "Hz: " <<Hz<<std::endl;
                Hz=0;
            }

            pluginsRunning(runTime);
            if (delayUs!=0) {
                std::this_thread::sleep_for(std::chrono::microseconds(delayUs));
            }
        }
        std::cout <<"close" <<std::endl;
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
        sync.sendRequest([this](){setRunning(false);},Sync::Event::CLOSE_EVENT);
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

bool Backend::resetTarge() {
    bool res = true;
    sync.sendRequest([this,&res]() {
        res = daplink->resetTarget();
    },Sync::Event::RESET_EVENT);
    return res;
}

void Backend::requestHandler() {
    sync.tryGetRequest([this](Sync::Event e) {
        switch (e) {
            case Sync::Event::UPDATE_VARI_EVENT:
                daplink->resetMap(pluginContainer);
                qDebug() << "vari"<<QDateTime::currentDateTimeUtc();
                break;
            case Sync::Event::WRITE_EVENT:
                qDebug() << "write"<<QDateTime::currentDateTimeUtc();
                break;
            case Sync::Event::RESET_EVENT:
                qDebug() << "reset"<<QDateTime::currentDateTimeUtc();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                break;
            case Sync::Event::CLOSE_EVENT:
                qDebug() << "close"<<QDateTime::currentDateTimeUtc();
                break;
        }
    });
}

void Backend::pluginsRunning(qreal runTime) {
    for (auto& plugin: pluginContainer) {
        plugin->onPluginRunning(runTime);
    }
}

void Backend::pluginsStart() {
    for (auto& plugin: pluginContainer) {
        plugin->onPluginStart();
    }
}

void Backend::pluginsEnd() {
    for (auto& plugin: pluginContainer) {
        plugin->onPluginEnd();
    }
}

void Backend::pushPlugin(DisplayPluginInterface *plugin) {
    pluginContainer.push_back(plugin);
}

void Backend::erasePlugin(DisplayPluginInterface *plugin) {
    std::erase_if(pluginContainer, [plugin](void *ptr)->bool{return ptr==plugin;});
}
