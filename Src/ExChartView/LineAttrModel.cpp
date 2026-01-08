//
// Created by liaohy on 12/27/25.
//

#include "LineAttrModel.hpp"

#include "ExChartView.hpp"

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
            return attr->config.user.name;
        case VariType:
            return attr->config.user.type;
        case VariAddr:
            return attr->config.user.addr;
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
    };
}

bool LineAttrModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()) return {};
    auto& attr = lineAttrs[index.row()];
    bool changed = false;

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
            }
            break;
        case VisibleRole:
            if (attr->config.visible!=value.toBool()) {
                attr->config.visible= value.toBool();
                changed = true;
            }
            break;
        case BufCapRole:
            if (attr->config.capacity!=value.toUInt()) {
                attr->config.capacity = value.toUInt();
                changed = true;
            }
            break;
        case BufLenRole:
            if (attr->view.pointsLen!=value.toUInt()) {
                attr->view.pointsLen = value.toUInt();
                changed = true;
                qDebug()<<"attr.view.pointsLen: "<<attr->view.pointsLen;
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
            if (attr->config.user.name!=value.toString()) {
                attr->config.user.name = value.toString();
                changed = true;
            }
            break;
        case VariType:
            if (attr->config.user.type!=value.toInt()) {
                attr->config.user.type = static_cast<BasicType>(value.toInt());
                changed = true;
            }
            break;
        case VariAddr:
            if (attr->config.user.addr!=value.toUInt()) {
                attr->config.user.addr = value.toUInt();
                changed = true;
            }
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

void LineAttrModel::appendLine(const QString &name, const QColor &color, quint32 bufferCapacity, QVariant node) {
    auto t = std::make_shared<LineAttr>(LineAttr{.group = "null", .config = {.color = color, .name = name, .visible = true, .capacity = bufferCapacity, .user = {}}, .view = {}});
    int row = lineAttrs.size();
    beginInsertRows(QModelIndex(),row,row);
    lineAttrs.emplace_back(t);
    reinterpret_cast<ExChartView *>(this->parent())->onAttrPushed(get<VariNode*>(node));
    endInsertRows();
}

void LineAttrModel::removeLine(int index) {
    beginRemoveRows(QModelIndex(),index,index);
    lineAttrs.remove(index);
    reinterpret_cast<ExChartView *>(this->parent())->onAttrRemoved(index);
    endRemoveRows();
}
