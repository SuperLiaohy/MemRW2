//
// Created by liaohy on 12/25/25.
//


#include "ChartLine.hpp"

#include <QPainterPath>


void ChartLine::setLineName(const QString &name) {
    lineName = name;
}

void ChartLine::setCapacity(std::size_t cap) {
    bufferCapacity = cap;
}

void ChartLine::setColor(const QColor &color) {
    lineColor = color;
}

void ChartLine::clearData() {
    writeHandle = 0;
    dataLen = 0;
}

QPointF ChartLine::at(std::size_t index) const {
    if (bufferCapacity != dataLen) return buffer[index];
    return buffer[(writeHandle + index) % bufferCapacity];
}

void ChartLine::write(const QPointF &point) {
    buffer[writeHandle] = point;
    writeHandle = (writeHandle + 1) % bufferCapacity;
    if (bufferCapacity > dataLen) ++dataLen;
}

void ChartLine::writeBuffer(const QVector<QPointF> &points) {
    for (const auto & point: points) {write(point);}
}

void ChartLine::paint(QPainter *painter) {
    readyForPath();
    if (pathPoint.size() < 2) {
        return;
    }
    painter->setRenderHint(QPainter::Antialiasing);
    QPen pen(lineColor);
    pen.setWidthF(lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter->setPen(pen);
    QPainterPath path;
    path.moveTo(pathPoint.first());
    for (int i = 1; i < pathPoint.size(); ++i) {
        path.lineTo(pathPoint[i]);
    }
    painter->drawPath(path);
}

void ChartLine::readyForPath() {
    pathPoint.resize(dataLen);
    if (dataLen!=bufferCapacity) {
        memcpy(pathPoint.data(),buffer.data(),dataLen*sizeof(QPointF));
    } else {
        memcpy(pathPoint.data(),&buffer[writeHandle],bufferCapacity-writeHandle*sizeof(QPointF));
        if (writeHandle!=0)
            memcpy(&pathPoint[bufferCapacity-writeHandle],&buffer[0],writeHandle*sizeof(QPointF));
    }
}
