//
// Created by liaohy on 12/27/25.
//

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include "ExChartView.hpp"

void ExLine::write(const QPointF &point) {
    buf[writeHandle] = point;
    writeHandle = (writeHandle + 1) % capacity;
    if (capacity > len) ++len;
}

void ExLine::writeBuffer(const QVector<QPointF> &points) {
    for (const auto & point: points) {write(point);}
}

QPointF ExLine::at(quint32 index) const {
    if (capacity > len) return buf[index];
    return buf[(writeHandle + index) % capacity];
}

ExChartView::ExChartView(QQuickItem *parent) : QQuickItem(parent) {
    lineAttrModel = new LineAttrModel(this);
    connect(lineAttrModel, &LineAttrModel::dataChanged,this,[this](const QModelIndex &topLeft, const QModelIndex &bottomRight,const QList<int> &roles) {
        qDebug()<<"data changed" << topLeft<<bottomRight<<roles;
    });
    connect(lineAttrModel, &LineAttrModel::rowsInserted,this,[this](const QModelIndex &parent, int first, int last) {
        for (int i = 0; i < last-first+1; ++i) {
            lines.append(ExLine{lineAttrModel->lineAttrs[first+i]->config.capacity});
            bufA.append(PointBuf{});
            bufB.append(PointBuf{});
        }
    });
    connect(lineAttrModel, &LineAttrModel::rowsRemoved,this,[this](const QModelIndex &parent, int first, int last) {
        lines.remove(first,last-first+1);
        bufA.remove(first,last-first+1);
        bufB.remove(first,last-first+1);
    });
    connect(lineAttrModel, &LineAttrModel::modelReset,this,[](){});
}

QSGNode * ExChartView::updatePaintNode(QSGNode *qsg_node, UpdatePaintNodeData *update_paint_node_data) {
    QSGNode* root = qsg_node?qsg_node:new QSGNode;
    QSGGeometryNode* node = nullptr;    QSGGeometry *geom = nullptr;
    for (int i = 0; i < lines.size(); ++i) {
        node = static_cast<QSGGeometryNode *>(root->childAtIndex(i));
        if (!node) {
            node = new QSGGeometryNode;
            geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), lines[i].getLen());
            geom->setDrawingMode(QSGGeometry::DrawLineStrip);
            geom->setLineWidth(3.0);
            node->setGeometry(geom);
            node->setFlag(QSGNode::OwnsGeometry);
            QSGFlatColorMaterial *mat = new QSGFlatColorMaterial;
            mat->setColor(lineAttrModel->lineAttrs[i]->config.color);
            node->setMaterial(mat);
            node->setFlag(QSGNode::OwnsMaterial);

            root->appendChildNode(node);
        } else {
            geom = node->geometry();
            if (geom->vertexCount()!=lines[i].getLen()) geom->allocate(lines[i].getLen());
        }
        QSGGeometry::Point2D *v = geom->vertexDataAsPoint2D();
        auto xRange = viewXMax - viewXMin;
        auto yRange = viewYMax - viewYMin;
        auto w = width();
        auto h = height();
        for (int j = 0; j < lines[i].getLen(); ++j) {
            auto x = (lines[i].at(j).x() - viewXMin)/xRange*w;
            auto y = (viewYMax - lines[i].at(j).y())/yRange*h;
            v[j].set(x,y);
        }
        node->markDirty(QSGNode::DirtyGeometry);
    }
    return root;
}

