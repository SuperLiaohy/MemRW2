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
    ExLine(quint32 cap):writeHandle(0),len(0),capacity(cap){buf.resize(cap);}
    bool deleteLater = false;
    quint32 getLen() {return len;}
    void write(const QPointF& point);
    void writeBuffer(const QVector<QPointF>& points);
    [[nodiscard]] QPointF at(quint32 index) const;
};

class ExChartView : public QQuickItem {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(LineAttrModel* lineAttrModel READ getLineAttrModel NOTIFY lineAttrModelChanged)
    Q_PROPERTY(qreal viewXMax READ getViewXMax WRITE setViewXMax NOTIFY viewXMaxChanged)
    Q_PROPERTY(qreal viewXMin READ getViewXMin WRITE setViewXMin NOTIFY viewXMinChanged)
    Q_PROPERTY(qreal viewYMin READ getViewYMin WRITE setViewYMin NOTIFY viewYMinChanged)
    Q_PROPERTY(qreal viewYMax READ getViewYMax WRITE setViewYMax NOTIFY viewYMaxChanged)

    Q_PROPERTY(qreal viewXRange READ getViewXRange WRITE setViewXRange NOTIFY viewXRangeChanged)
    Q_PROPERTY(qreal viewYRange READ getViewYRange WRITE setViewYRange NOTIFY viewYRangeChanged)

public:
    explicit ExChartView(QQuickItem* parent = nullptr);
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;
    void switchBuf();
    LineAttrModel* getLineAttrModel() {return lineAttrModel;}
    std::atomic<QVector<PointBuf>*> backBuf;

    qreal getViewXMax(){ return viewXMax;}
    qreal getViewXMin(){ return viewXMin;}
    qreal getViewYMin(){ return viewYMin;}
    qreal getViewYMax(){ return viewYMax;}
    qreal getViewXRange(){ return viewXRange;}
    qreal getViewYRange(){ return viewYRange;}

    void setViewXMax(qreal value){viewXMax = value; emit viewXMaxChanged();}
    void setViewXMin(qreal value){viewXMin = value; emit viewXMinChanged();}
    void setViewYMin(qreal value){viewYMin = value; emit viewYMinChanged();}
    void setViewYMax(qreal value){viewYMax = value; emit viewYMaxChanged();}
    void setViewXRange(qreal range){ viewXRange = range; emit viewXRangeChanged();}
    void setViewYRange(qreal range){ viewYRange = range; emit viewYRangeChanged();}

    void onAttrChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,const QList<int> &roles);
    void onAttrRemoved(int index);
signals:
    void lineAttrModelChanged();
    void viewXMaxChanged();
    void viewXMinChanged();
    void viewYMinChanged();
    void viewYMaxChanged();
    void viewXRangeChanged();
    void viewYRangeChanged();
private:
    qreal viewXRange = 5000;
    qreal viewYRange = 100;

    qreal viewXMax = viewXRange;
    qreal viewXMin = 0;
    qreal viewYMax = viewYRange;
    qreal viewYMin = 0;

    QVector<ExLine> lines;
    LineAttrModel* lineAttrModel;
    QVector<PointBuf> bufA;
    QVector<PointBuf> bufB;
};
