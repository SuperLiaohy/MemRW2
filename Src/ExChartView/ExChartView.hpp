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
    void setCapacity(quint32 c) {buf.resize(c); clear(); capacity = c;}
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
    Q_PROPERTY(qreal viewXCenter READ getViewXCenter WRITE setViewXCenter NOTIFY viewXCenterChanged)
    Q_PROPERTY(qreal viewYCenter READ getViewYCenter WRITE setViewYCenter NOTIFY viewYCenterChanged)

    Q_PROPERTY(quint32 targetFps READ getTargetFps WRITE setTargetFps NOTIFY targetFpsChanged)
    Q_PROPERTY(quint32 realFps READ getRealFps NOTIFY realFpsChanged)

    Q_PROPERTY(bool flow READ getFlow WRITE setFlow NOTIFY flowChanged)

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
    [[nodiscard]] qreal getViewXCenter() const{ return viewXCenter;}
    [[nodiscard]] qreal getViewYCenter() const{ return viewYCenter;}
    [[nodiscard]] quint32 getTargetFps() const{ return targetFps;}
    [[nodiscard]] quint32 getRealFps() const{ return realFps;}
    [[nodiscard]] bool getFlow() const{ return flow;}

    void setViewXMax(qreal value){viewXMax = value; emit viewXMaxChanged();}
    void setViewXMin(qreal value){viewXMin = value; emit viewXMinChanged();}
    void setViewYMin(qreal value){viewYMin = value; emit viewYMinChanged();}
    void setViewYMax(qreal value){viewYMax = value; emit viewYMaxChanged();}
    void setViewXRange(qreal range){ viewXRange = range; emit viewXRangeChanged();}
    void setViewYRange(qreal range){ viewYRange = range; emit viewYRangeChanged();}
    void setViewXCenter(qreal value){ viewXCenter = value; emit viewXCenterChanged();}
    void setViewYCenter(qreal value){ viewYCenter = value; emit viewYCenterChanged();}
    void setTargetFps(quint32 fps){ targetFps = fps; emit targetFpsChanged();}
    void setFlow(bool isFlow){ flow = isFlow; emit flowChanged();}

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
    void viewXCenterChanged();
    void viewYCenterChanged();
    void targetFpsChanged();
    void realFpsChanged();
    void flowChanged();
    void timingUpdate(qreal runTime);
private:
    friend bool LineAttrModel::setData(const QModelIndex &index, const QVariant &value, int role);
    qreal viewXRange = 5000;
    qreal viewYRange = 1000;
    qreal viewXCenter = 2500;
    qreal viewYCenter = 0;

    qreal viewXMax = viewXCenter + viewXRange / 2;
    qreal viewXMin = viewXCenter - viewXRange / 2;
    qreal viewYMax = viewYCenter + viewYRange / 2;
    qreal viewYMin = viewYCenter - viewYRange / 2;

    quint32 targetFps;
    quint32 frameCount=0;
    quint32 realFps=0;
    bool flow = true;

    QVector<ExLine> lines;
    LineAttrModel* lineAttrModel;
    QVector<PointBuf> bufA;
    QVector<PointBuf> bufB;
    qreal lastUpdateTime = 0;
    qreal everySec = 0;
};
