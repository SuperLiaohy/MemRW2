//
// Created by liaohy on 12/27/25.
//

#include "LineAttrModel.hpp"

#include "ExChartView.hpp"
#include "VariComponent.hpp"

int LineAttrModel::rowCount(const QModelIndex &parent) const {
    return lineAttrs.size();
}

QVariant LineAttrModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};
    auto& attr = lineAttrs[index.row()];
    switch (role) {
        case NameRole:
            return attr->config.name;
        case ColorRole:
            return attr->config.color;
        case VisibleRole:
            return attr->config.visible;
        case BufCapRole:
            return attr->config.capacity;
        case BufLenRole:
            return attr->view.pointsLen;
        case PaintLenRole:
            return attr->view.paintCount;
        case GroupRole:
            return attr->group;
        case VariName:
            return reinterpret_cast<ExChartView*>(this->parent())->variContainer[index.row()]->getName();
        case VariType:
            return reinterpret_cast<ExChartView*>(this->parent())->variContainer[index.row()]->getTypeName();
        case VariAddr:
            return static_cast<qlonglong>(reinterpret_cast<ExChartView*>(this->parent())->variContainer[index.row()]->getAddress());
        case VariSize:
            return static_cast<qlonglong>(reinterpret_cast<ExChartView*>(this->parent())->variContainer[index.row()]->getSize());
        default:
            return {};
    }
}

QHash<int, QByteArray> LineAttrModel::roleNames() const {
    return {
        {NameRole, "name"},
        {ColorRole, "color"},
        {VisibleRole, "visible"},
        {BufCapRole, "capacity"},
        {BufLenRole, "len"},
        {PaintLenRole, "paintLen"},
        {GroupRole, "group"},
        {VariName, "variName"},
        {VariType, "variType"},
        {VariAddr, "variAddr"},
        {VariSize, "variSize"},
    };
}

bool LineAttrModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) return {};
    auto& attr = lineAttrs[index.row()];
    bool changed = false;
    auto& line  = reinterpret_cast<ExChartView*>(this->parent())->lines[index.row()];

    switch (role) {
        case NameRole:
            if (attr->config.name!=value.toString()) {
                attr->config.name = value.toString();
                changed = true;
            }
            break;
        case ColorRole:
            if (attr->config.color!=value.toString()) {
                attr->config.color = value.toString();
                changed = true;
                reinterpret_cast<ExChartView*>(this->parent())->update();
            }
            break;
        case VisibleRole:
            if (attr->config.visible!=value.toBool()) {
                attr->config.visible= value.toBool();
                changed = true;
                reinterpret_cast<ExChartView*>(this->parent())->update();
            }
            break;
        case BufCapRole:
            if (attr->config.capacity!=value.toUInt()) {
                attr->config.capacity = value.toUInt();
                line.setCapacity(attr->config.capacity);
                changed = true;
                reinterpret_cast<ExChartView*>(this->parent())->update();
            }
            break;
        case BufLenRole:
            if (attr->view.pointsLen!=value.toUInt()) {
                attr->view.pointsLen = value.toUInt();
                changed = true;
            }
            break;
        case PaintLenRole:
            if (attr->view.paintCount!=value.toUInt()) {
                attr->view.paintCount = value.toUInt();
                changed = true;
            }
            break;
        case GroupRole:
            if (attr->group!=value.toString()) {
                attr->group = value.toString();
                changed = true;
            }
            break;
        case VariName:
            // if (attr->config.user.name!=value.toString()) {
            //     attr->config.user.name = value.toString();
            //     changed = true;
            // }
            break;
        case VariType:
            // if (attr->config.user.type!=value.toInt()) {
            //     attr->config.user.type = static_cast<BasicType>(value.toInt());
            //     changed = true;
            // }
            break;
        case VariAddr:
            // if (attr->config.user.addr!=value.toUInt()) {
            //     attr->config.user.addr = value.toUInt();
            //     changed = true;
            // }
            break;
        default:
            return false;
    }
    if (changed) emit dataChanged(index,index,{role});
    return changed;
}

Qt::ItemFlags LineAttrModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

void LineAttrModel::appendLine(const QString &name, const QColor &color, quint32 bufferCapacity, const QString&variName , const QString& type, std::size_t address, std::size_t size) {
    auto t = std::make_shared<LineAttr>(LineAttr{.group = "null", .config = {.color = color, .name = name, .visible = true, .capacity = bufferCapacity, }, .view = {}});
    int row = lineAttrs.size();
    beginInsertRows(QModelIndex(),row,row);
    lineAttrs.emplace_back(t);
    reinterpret_cast<ExChartView *>(this->parent())->onAttrPushed(variName,type,address,size);
    endInsertRows();
}

void LineAttrModel::removeLine(int index) {
    beginRemoveRows(QModelIndex(),index,index);
    lineAttrs.remove(index);
    reinterpret_cast<ExChartView *>(this->parent())->onAttrRemoved(index);
    endRemoveRows();
}
