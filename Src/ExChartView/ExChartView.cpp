//
// Created by liaohy on 12/27/25.
//

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QtAlgorithms>
#include "ExChartView.hpp"
#include "Backend.hpp"

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
    setFlag(QQuickItem::ItemHasContents, true);
    lineAttrModel = new LineAttrModel(this);
    backBuf.store(&bufA);
    Backend::instance().setChartView(this);
    connect(lineAttrModel, &LineAttrModel::dataChanged,this,[this](const QModelIndex &topLeft, const QModelIndex &bottomRight,const QList<int> &roles) {
        qDebug()<<"data changed" << topLeft<<bottomRight<<roles;
        onAttrChanged(topLeft,bottomRight,roles);
    });
    connect(lineAttrModel, &LineAttrModel::rowsInserted,this,[this](const QModelIndex &parent, int first, int last) {
        for (int i = 0; i < last-first+1; ++i) {
            lines.append(ExLine{lineAttrModel->lineAttrs[first+i]->config.capacity});
            bufA.append(PointBuf{});
            bufB.append(PointBuf{});
        }
    });
    connect(lineAttrModel, &LineAttrModel::rowsRemoved,this,[this](const QModelIndex &parent, int first, int last) {
        for (int i = first; i <=last; ++i) {
            lines[i].deleteLater = true;
        }
        update();
        // Backend::instance().sync.sendRequest([this,first,last]() {
        //     lines.remove(first,last-first+1);
        //     bufA.remove(first,last-first+1);
        //     bufB.remove(first,last-first+1);
        //     update();
        // });
    });
    connect(lineAttrModel, &LineAttrModel::modelReset,this,[](){});
}

QSGNode * ExChartView::updatePaintNode(QSGNode *qsg_node, UpdatePaintNodeData *update_paint_node_data) {
    qDebug()<<"paint";
    QSGNode* root = qsg_node?qsg_node:new QSGNode;
    QSGGeometryNode* node = nullptr;    QSGGeometry *geom = nullptr;
    QSet<int> deleteIndexes;
    int realIndex=0;

    for (int i = 0; i < lines.size(); ++i) {
        node = static_cast<QSGGeometryNode *>(root->childAtIndex(realIndex));
        if (lines[i].deleteLater) {
            deleteIndexes.insert(i);
            if (node) {
                root->removeChildNode(node);
                delete node;
                node=nullptr;
            }
            continue;
        }
        ++realIndex;
    }
    if (!deleteIndexes.empty()) {
        qDebug()<<root->childCount();
        Backend::instance().sync.sendRequest([this,&deleteIndexes]() {
            int idx = 0;
            erase_if(lines, [&deleteIndexes,&idx](auto&){return deleteIndexes.contains(idx++);});
            idx=0;
            erase_if(bufA, [&deleteIndexes,&idx](auto&){return deleteIndexes.contains(idx++);});
            idx=0;
            erase_if(bufB, [&deleteIndexes,&idx](auto&){return deleteIndexes.contains(idx++);});
        });
    }
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
            auto mat = static_cast<QSGFlatColorMaterial *>(node->material());
            mat->setColor(lineAttrModel->lineAttrs[i]->config.color);
            if (geom->vertexCount() != lines[i].getLen()) geom->allocate(lines[i].getLen());
        }

        if (lineAttrModel->lineAttrs[i]->config.visible) {
            node->setInheritedOpacity(1);
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
        } else {
            node->setInheritedOpacity(0);
        }
        node->markDirty(QSGNode::DirtyGeometry);
    }
    return root;
}

void ExChartView::switchBuf() {
    QVector<PointBuf>& ready = *backBuf.load();
    auto tmp = backBuf==&bufA?&bufB:&bufA;
    backBuf.store(tmp);
    for (int i = 0; i < lines.size(); ++i) {
        lines[i].writeBuffer(ready[i]);
        ready[i].clear();
    }
}

void ExChartView::onAttrChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles) {
    if (topLeft.row()!=bottomRight.row())return;
    if (roles.empty())return;
    auto role = roles[0];
    switch (role) {
        case LineAttrModel::NameRole:
            break;
        case LineAttrModel::ColorRole:
            update();
            break;
        case LineAttrModel::VisibleRole:
            update();
            break;
        case LineAttrModel::BufCapRole:
            break;
        case LineAttrModel::BufLenRole:
            break;
        case LineAttrModel::PaintLenRole:
            break;
        case LineAttrModel::GroupRole:
            break;
        case LineAttrModel::VariName:
            break;
        case LineAttrModel::VariType:
            break;
        case LineAttrModel::VariAddr:
            break;
        default:
            break;
    }
}

void ExChartView::onAttrRemoved(int index) {
    lines[index].deleteLater = true;
    update();
}

