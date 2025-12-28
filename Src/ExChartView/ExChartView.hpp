//
// Created by liaohy on 12/27/25.
//

#pragma once

#include <QQuickPaintedItem>
#include <QQuickItem>
#include "LineAttrModel.hpp"

using PointBuf = QVector<QPointF>;

class ExLine {
    PointBuf buf;
    quint32 writeHandle;
    quint32 len;
    quint32 capacity;
public:
    ExLine(quint32 cap):writeHandle(0),len(0),capacity(cap){}
    quint32 getLen() {return len;}
    void write(const QPointF& point);
    void writeBuffer(const QVector<QPointF>& points);
    [[nodiscard]] QPointF at(quint32 index) const;
};

class ExChartView : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
public:
    explicit ExChartView(QQuickItem* parent = nullptr);
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
private:
    qreal viewXMax;
    qreal viewXMin;
    qreal viewYMin;
    qreal viewYMax;

    QVector<ExLine> lines;
    LineAttrModel* lineAttrModel;
    QVector<PointBuf> bufA;
    QVector<PointBuf> bufB;
    std::atomic<QVector<PointBuf>*> backBuf;


};
