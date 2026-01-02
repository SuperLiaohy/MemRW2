//
// Created by liaohy on 12/29/25.
//

#include "Backend.hpp"

#include "ExChartView.hpp"

void Backend::init() {
    samplingWorker = std::jthread([this](const std::stop_token& stoken) {
        bool lastRun = running;
        auto startSysTime = std::chrono::high_resolution_clock::now();
        auto everyFrameSysTime = std::chrono::high_resolution_clock::now();
        qreal Time = 0;
        while (!stoken.stop_requested()) {
            sync.tryGetRequest([this]() {
                qDebug() << "remove line";
            });
            if (lastRun==false&&running==true) {
                startSysTime = std::chrono::high_resolution_clock::now();
                everyFrameSysTime = startSysTime;
                Time = 0;
            }
            lastRun = running;
            if (!running) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            // start to operate on charts
            auto nowSysTime = std::chrono::high_resolution_clock::now();
            runTime = std::chrono::duration_cast<std::chrono::microseconds>((nowSysTime-startSysTime)).count()/1000.0;
            int index = 0;
            for (auto & buf: *chartView->backBuf) {
                auto point = QPointF(runTime,(++index*10)+0.5*chartView->getViewYRange()+0.3*chartView->getViewYRange()*std::sin(runTime/300*2*std::numbers::pi));
                buf.append(point);
            }

            Time = std::chrono::duration_cast<std::chrono::microseconds>((nowSysTime-everyFrameSysTime)).count()/1000.0;
            if (Time>30) {
                chartView->switchBuf();
                emit updatePath(runTime);
                qDebug()<<"update "<<"xMin:"<<chartView->getViewXMin() << "xMax:"<<chartView->getViewXMax()
                <<"yMin:"<<chartView->getViewYMin()<<"yMax:"<<chartView->getViewYMax();
                everyFrameSysTime = nowSysTime;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
}
