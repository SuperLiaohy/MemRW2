//
// Created by liaohy on 12/27/25.
//

#pragma once

#include <QQuickPaintedItem>
#include <QQuickItem>

#include "DisplayPluginInterface.hpp"
#include "LineAttrModel.hpp"

using PointBuf = QVector<QPointF>;

class ExLine {
    PointBuf buf;
    quint32 writeHandle;
    quint32 len;
    quint32 capacity;
public:
    ExLine(quint32 cap):writeHandle(0),len(0),capacity(cap){buf.resize(cap);}
    bool deleteLater = false;
    quint32 getLen() {return len;}
    quint32 getCapacity() {return capacity;}
    void write(const QPointF& point);
    void writeBuffer(const QVector<QPointF>& points);
    [[nodiscard]] QPointF at(quint32 index) const;
    void clear() {writeHandle=0,len=0;};
    std::pair<std::size_t,std::size_t> findBoundary(qreal viewXMin,qreal viewXMax);
    std::size_t findPointIndex(qreal x);
};

class ExChartView : public QQuickItem, public DisplayPluginInterface {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(LineAttrModel* lineAttrModel READ getLineAttrModel NOTIFY lineAttrModelChanged)
    Q_PROPERTY(qreal viewXMax READ getViewXMax WRITE setViewXMax NOTIFY viewXMaxChanged)
    Q_PROPERTY(qreal viewXMin READ getViewXMin WRITE setViewXMin NOTIFY viewXMinChanged)
    Q_PROPERTY(qreal viewYMin READ getViewYMin WRITE setViewYMin NOTIFY viewYMinChanged)
    Q_PROPERTY(qreal viewYMax READ getViewYMax WRITE setViewYMax NOTIFY viewYMaxChanged)

    Q_PROPERTY(qreal viewXRange READ getViewXRange WRITE setViewXRange NOTIFY viewXRangeChanged)
    Q_PROPERTY(qreal viewYRange READ getViewYRange WRITE setViewYRange NOTIFY viewYRangeChanged)

    Q_PROPERTY(quint32 targetFps READ getTargetFps WRITE setTargetFps NOTIFY targetFpsChanged)
    Q_PROPERTY(quint32 realFps READ getRealFps NOTIFY realFpsChanged)

public:
    explicit ExChartView(QQuickItem* parent = nullptr);
    ~ExChartView();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void switchBuf();
    void updatePath(qreal runTime);
    void updateData(qreal runTime) override;
    void clearData() override;
    LineAttrModel *getLineAttrModel() {return lineAttrModel;}
    std::atomic<QVector<PointBuf>*> backBuf;
    std::atomic<QVector<PointBuf>*> frontBuf;

    [[nodiscard]] qreal getViewXMax() const{ return viewXMax;}
    [[nodiscard]] qreal getViewXMin() const{ return viewXMin;}
    [[nodiscard]] qreal getViewYMin() const{ return viewYMin;}
    [[nodiscard]] qreal getViewYMax() const{ return viewYMax;}
    [[nodiscard]] qreal getViewXRange() const{ return viewXRange;}
    [[nodiscard]] qreal getViewYRange() const{ return viewYRange;}
    [[nodiscard]] quint32 getTargetFps() const{ return targetFps;}
    [[nodiscard]] quint32 getRealFps() const{ return realFps;}

    void setViewXMax(qreal value){viewXMax = value; emit viewXMaxChanged();}
    void setViewXMin(qreal value){viewXMin = value; emit viewXMinChanged();}
    void setViewYMin(qreal value){viewYMin = value; emit viewYMinChanged();}
    void setViewYMax(qreal value){viewYMax = value; emit viewYMaxChanged();}
    void setViewXRange(qreal range){ viewXRange = range; emit viewXRangeChanged();}
    void setViewYRange(qreal range){ viewYRange = range; emit viewYRangeChanged();}
    void setTargetFps(quint32 fps){ targetFps = fps; emit targetFpsChanged();}

    void onAttrChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,const QList<int> &roles);
    void onAttrRemoved(int index);
    void onAttrPushed(const QString&name , const QString& type, std::size_t address, std::size_t size);
signals:
    void lineAttrModelChanged();
    void viewXMaxChanged();
    void viewXMinChanged();
    void viewYMinChanged();
    void viewYMaxChanged();
    void viewXRangeChanged();
    void viewYRangeChanged();
    void targetFpsChanged();
    void realFpsChanged();
    void timingUpdate(qreal runTime);
private:
    qreal viewXRange = 5000;
    qreal viewYRange = 100;

    qreal viewXMax = viewXRange;
    qreal viewXMin = 0;
    qreal viewYMax = viewYRange;
    qreal viewYMin = 0;

    quint32 targetFps;
    quint32 frameCount=0;
    quint32 realFps=0;

    QVector<ExLine> lines;
    LineAttrModel* lineAttrModel;
    QVector<PointBuf> bufA;
    QVector<PointBuf> bufB;
    qreal lastUpdateTime = 0;
    qreal everySec = 0;
};
