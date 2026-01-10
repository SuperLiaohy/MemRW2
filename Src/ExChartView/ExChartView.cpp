//
// Created by liaohy on 12/27/25.
//

#include <QSGGeometryNode>
#include <QSGGeometry>
#include <QSGFlatColorMaterial>
#include <QtAlgorithms>
#include <QTimer>
#include "ExChartView.hpp"
#include "Backend.hpp"
#include "VariComponent.hpp"

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

std::pair<std::size_t, std::size_t> ExLine::findBoundary(qreal viewXMin, qreal viewXMax) {
    if (len==0) {return {0,0};}
    if (at(len-1).x()<viewXMin) {return{len-1,len-1};}
    if (at(0).x()>viewXMax) {return{0,0};}

    std::size_t lowBound = 0;
    std::size_t highBound = len;

    if (at(0).x()<viewXMin) {
        std::size_t low = 0, high = len;
        while (low < high) {
            std::size_t mid = low + (high - low) / 2;
            if (at(mid).x() < viewXMin) {
                low = mid + 1;
            } else {
                high = mid;
            }
        }
        lowBound = low;
    }
    if (at(len-1).x()>viewXMax) {
        std::size_t low = 0, high = len;
        while (low < high) {
            std::size_t mid = low + (high - low) / 2;
            if (at(mid).x() <= viewXMax) {
                low = mid + 1;
            } else {
                high = mid;
            }
        }
        highBound = low;
    }
    if (lowBound > highBound) {
        return {0, 0};
    }
    return {lowBound,highBound};
}

std::size_t ExLine::findPointIndex(qreal x) {
}

ExChartView::ExChartView(QQuickItem *parent) : QQuickItem(parent), lineAttrModel(new LineAttrModel(this)), backBuf(&bufA) {
    setFlag(QQuickItem::ItemHasContents, true);

    for (auto & buf: bufA) {
        buf.reserve(256);
    }
    for (auto & buf: bufB) {
        buf.reserve(256);
    }

    connect(this,&ExChartView::timingUpdate,this,&ExChartView::updatePath);

    QTimer* timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer,&QTimer::timeout,this,[this](){realFps=frameCount;frameCount=0;emit realFpsChanged();});
    timer->start();
}

ExChartView::~ExChartView() {
    Backend::instance().sync.sendRequest([](){Backend::instance().setRunning(false);});
}

QSGNode * ExChartView::updatePaintNode(QSGNode *qsg_node, UpdatePaintNodeData *update_paint_node_data) {
    ++frameCount;
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
            eraseUnits(deleteIndexes);
        });
    }
    std::size_t lowBound=0,highBound=0,range=0;
    for (int i = 0; i < lines.size(); ++i) {
        node = static_cast<QSGGeometryNode *>(root->childAtIndex(i));
        std::tie(lowBound,highBound) = lines[i].findBoundary(viewXMin,viewXMax);
        range = highBound-lowBound;
        if (!node) {
            node = new QSGGeometryNode;
            geom = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), lines[i].getCapacity());
            geom->setVertexDataPattern(QSGGeometry::StreamPattern);
            geom->setVertexCount(range);
            geom->setDrawingMode(QSGGeometry::DrawLineStrip);
            geom->setLineWidth(3.0);
            node->setGeometry(geom);
            node->setFlag(QSGNode::OwnsGeometry);
            QSGFlatColorMaterial *mat = new QSGFlatColorMaterial;
            mat->setColor(lineAttrModel->lineAttrs[i]->config.color);
            node->setMaterial(mat);
            node->setFlag(QSGNode::OwnsMaterial);
            root->appendChildNode(node);
            qDebug()<<"paint";
        } else {
            geom = node->geometry();
            auto mat = static_cast<QSGFlatColorMaterial *>(node->material());
            mat->setColor(lineAttrModel->lineAttrs[i]->config.color);
            if (geom->vertexCount() != range) geom->setVertexCount(range);
        }

        if (lineAttrModel->lineAttrs[i]->config.visible) {
            node->setInheritedOpacity(1);
            QSGGeometry::Point2D *v = geom->vertexDataAsPoint2D();
            auto xRange = viewXMax - viewXMin;
            auto yRange = viewYMax - viewYMin;
            auto w = width();
            auto h = height();
            for (std::size_t j = lowBound; j < highBound; ++j) {
                auto x = (lines[i].at(j).x() - viewXMin)/xRange*w;
                auto y = (viewYMax - lines[i].at(j).y())/yRange*h;
                v[j-lowBound].set(x,y);
            }
        } else {
            node->setInheritedOpacity(0);
        }
        node->markDirty(QSGNode::DirtyGeometry);
    }
    return root;
}

void ExChartView::switchBuf() {
    QVector<PointBuf>* ready = backBuf.load();
    auto tmp = backBuf==&bufA?&bufB:&bufA;
    backBuf.store(tmp);
    frontBuf.store(ready);
}

void ExChartView::updatePath(qreal runTime) {
    auto& ready = *frontBuf;
    for (int i = 0; i < lines.size(); ++i) {
        lines[i].writeBuffer(ready[i]);
        // lineAttrModel->lineAttrs[i]->view.pointsLen = lines[i].getLen();
        lineAttrModel->setData(lineAttrModel->index(i),lines[i].getLen(),LineAttrModel::RoleNames::BufLenRole);
        ready[i].clear();
    }
    if (flow) {
        if (runTime>getViewXRange()) {
            setViewXMax(runTime);
            setViewXMin(runTime-getViewXRange());
        }
    }
    update();
}

void ExChartView::updateData(qreal runTime) {
    int index = 0;
    for (auto & buf: *backBuf) {
        auto point = QPointF(runTime,variContainer[index++]->fValue);
        buf.append(point);
    }
    if (runTime-lastUpdateTime>(1000/targetFps) || runTime<lastUpdateTime) {
        switchBuf();
        emit timingUpdate(runTime);
        lastUpdateTime = runTime;
    }
}

void ExChartView::clearData() {
    for (auto & line: lines) {line.clear();}
    for (auto & buf: bufA) {buf.clear();}
    for (auto & buf: bufB) {buf.clear();}
    setViewXMin(viewXCenter-viewXRange/2);
    setViewXMax(viewXCenter+viewXRange/2);
    setViewYMin(viewYCenter-viewYRange/2);
    setViewYMax(viewYCenter+viewYRange/2);
}

void ExChartView::onAttrRemoved(int index) {
    lines[index].deleteLater = true;
    update();
}

void ExChartView::onAttrPushed(const QString&name , const QString& type, std::size_t address, std::size_t size) {
    lines.append(ExLine{lineAttrModel->lineAttrs.last()->config.capacity});
    bufA.append(PointBuf{});
    bufB.append(PointBuf{});
    pushUnit(name ,type, address, size);
}
