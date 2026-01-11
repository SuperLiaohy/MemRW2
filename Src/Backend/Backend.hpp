//
// Created by liaohy on 12/29/25.
//

#pragma once
#include <QQmlApplicationEngine>
#include <qqmlengine.h>
#include <thread>


#include "DAPReader.h"
#include "TreeModel.hpp"
class DisplayPluginInterface;
class ExChartView;

class Sync {
public:
    Sync() = default;
    template<typename Callback>
    void sendRequest(Callback fun) {sem_request.release();sem_response.acquire(); fun(); sem_resume.release();}
    template<typename Callback>
    void tryGetRequest(Callback fun) {if (sem_request.try_acquire()) {sem_response.release();sem_resume.acquire();fun();}}
private:
    std::binary_semaphore sem_request{0};     // A → B: 有请求了 (初始0)
    std::binary_semaphore sem_response{0};    // B → A: 已应答 (初始0)
    std::binary_semaphore sem_resume{0};      // A → B: 可以恢复运行了 (初始0)
};

class Clock {
public:
    void reset() {starTime=std::chrono::high_resolution_clock::now();}
    template<typename T = std::chrono::microseconds>
    std::size_t runTime() {return std::chrono::duration_cast<T>((std::chrono::high_resolution_clock::now()-starTime)).count();}
private:
    std::chrono::time_point<std::chrono::system_clock> starTime;
};

class Backend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool running READ getRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool connected READ getConnected NOTIFY connectedChanged)
    Q_PROPERTY(quint32 samplingHz READ getSamplingHz NOTIFY samplingHzChanged)
    Q_PROPERTY(quint32 delayUs READ getDelayUs WRITE setDelayUs NOTIFY delayUsChanged)
    Backend() : daplink(std::make_unique<DAPReader>()), variModel(std::make_unique<TreeModel>(std::make_shared<VariTree>())) {};
public:
    static Backend& instance() {static Backend backend;return backend;}
    static Backend* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        Q_UNUSED(jsEngine)Q_UNUSED(qmlEngine)
        qDebug()<<"qml get cpp static instance.";
        auto result = &instance();
        QQmlEngine::setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
        return result;
    }
    void init();
    void registerVariModel(const QQmlApplicationEngine&engine);
    bool getRunning() {return  running;}
    void setRunning(bool run) {running.store(run);emit runningChanged();}
    bool getConnected() const {return  connected;}
    quint32 getSamplingHz() const {return  samplingHz;}
    quint32 getDelayUs() const {return  delayUs;}
    void setDelayUs(quint32 value) {delayUs = value; emit delayUsChanged();}

    Q_INVOKABLE bool connect();
    Q_INVOKABLE void disconnect();
    Q_INVOKABLE QStringList reloadVari();

    Sync sync{};
    Clock clock{};
    void requestHandler();
    void updatePlugin(qreal runTime);
    void clearPluginData();

    void pushPlugin(DisplayPluginInterface *plugin);
    void erasePlugin(DisplayPluginInterface *plugin);

    QModelIndex findNode(const QString& nodeName) {return variModel->findNode(nodeName);};
signals:
    void runningChanged();
    void connectedChanged();
    void samplingHzChanged();
    void delayUsChanged();
    void linkErrorHappen();
    void updatePath(qreal runTime);

private:

    std::atomic<bool> running{false};
    bool connected{false};
    quint32 samplingHz{0};
    quint32 delayUs{0};
    std::jthread samplingWorker;
    std::vector<DisplayPluginInterface*> pluginContainer;
    std::unique_ptr<DAPReader> daplink;
    std::unique_ptr<TreeModel> variModel;

};
