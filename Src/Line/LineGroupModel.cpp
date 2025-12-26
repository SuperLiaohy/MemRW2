//
// Created by liaohy on 12/25/25.
//


#include "LineGroupModel.hpp"

LineGroupModel::LineGroupModel(QObject *parent) : QAbstractListModel(parent) {
}

LineGroupModel::~LineGroupModel() = default;

int LineGroupModel::rowCount(const QModelIndex &parent) const {
    return groups.size();
}

QVariant LineGroupModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    auto group = groups.at(index.row());
    switch (role) {
        case NameRole:
            return group.name;
        case VisibleRole:
            return group.lineModel->isAllVisible();
        case SelectedRole:
            return true;
        case ChildrenRole:
            return QVariant::fromValue(group.lineModel.get());
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> LineGroupModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[VisibleRole] = "visible";
    roles[SelectedRole] = "selected";
    roles[ChildrenRole] = "lineModel";
    return roles;
}

bool LineGroupModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()||index.row()>=groups.size()) return false;
    auto group = groups[index.row()];
    bool changed = false;
    bool update = false;
    switch (role) {
        case NameRole:
            if (group.name!=value.toString()) {
                group.name=value.toString();
                changed = true;
            }
            break;
        case VisibleRole:
            if (group.lineModel->isAllVisible()!=value.toBool()) {
                group.lineModel->setAllVisible(value.toBool());
                changed = true;
            }
            break;
        case SelectedRole:
            return false;
        case ChildrenRole:
            return false;
        default:
            return false;
    }
    if (changed)
        emit dataChanged(index,index,{role});
    // if (update)
    //     line->update();
    return changed;
}

Qt::ItemFlags LineGroupModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool LineGroupModel::appendGroup(const QString &groupName) {
    for (const auto & group: groups) {
        if (group.name==groupName)
            return false;
    }
    int row = groups.size();
    beginInsertRows(QModelIndex(),row,row);
    groups.emplace_back(groupName);
    endInsertRows();
    emit groupCountChanged();
    return true;
}

void LineGroupModel::removeGroup(int index) {
    auto before = lineCount();
    beginRemoveRows(QModelIndex(),index,index);
    groups.remove(index);
    endRemoveRows();
    emit groupCountChanged();
    if (before!=lineCount())emit lineCountChanged();
}
