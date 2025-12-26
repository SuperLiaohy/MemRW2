//
// Created by liaohy on 12/25/25.
//

#pragma once

#include <QString>
#include <QColor>
#include <QDebug>
#include <QPointF>
#include <QQuickPaintedItem>
#include <QPainter>
#include <QUuid>

class ChartLine : public QQuickPaintedItem {
public:
    explicit ChartLine(const QString& name, const QColor& color, std::size_t capacity = 5000)
        : lineId(QUuid::createUuid().toString())
        , lineColor(color)
        , lineName(name)
        , lineWidth(1.5)
        , bufferCapacity(capacity)
        , writeHandle(0)
        , dataLen(0)
    {
        buffer.resize(bufferCapacity);
        pathPoint.resize(bufferCapacity);
        qDebug() << "ChartLine created:" << "lineId:"<< lineId << "Name:" << lineName << "Color:" << lineColor.name();
    }

    ~ChartLine() {qDebug() << "ChartLineData destroyed:";}

    [[nodiscard]] const QString& getLineId() const { return lineId; }
    [[nodiscard]] const QString& getLineName() const { return lineName; }
    [[nodiscard]] const QColor& getLineColor() const { return lineColor; }
    [[nodiscard]] std::size_t getLen() const { return dataLen; }
    [[nodiscard]] std::size_t getCapacity() const { return bufferCapacity; }

    void setLineName(const QString& name);
    void setCapacity(std::size_t cap);
    void setColor(const QColor& color);
    void clearData();

    QPointF at(std::size_t index) const;
    void write(const QPointF& point);
    void writeBuffer(const QVector<QPointF>& points);
    void paint(QPainter *painter) override;
private:
    void readyForPath();
    QString lineId;
    QColor lineColor;
    QString lineName;
    qreal lineWidth;
    std::size_t bufferCapacity;

    QVector<QPointF> buffer;
    std::size_t writeHandle;
    std::size_t dataLen;
    QVector<QPointF> pathPoint;
};

