//
// Created by liaohy on 1/12/26.
//

#include "ExTableModel.h"

#include "Backend.hpp"
#include "VariComponent.hpp"

QStringList ExTableModel::headers{"name","read value", "write vaule","",""};

ExTableModel::ExTableModel(QObject *parent) {
}

ExTableModel::~ExTableModel() {
    Backend::instance().sync.sendRequest([]() {
        Backend::instance().setRunning(false);
    });
}

int ExTableModel::rowCount(const QModelIndex &parent) const {
    return rowData.size();
}

int ExTableModel::columnCount(const QModelIndex &parent) const {
    return 5;
}

QVariant ExTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= rowData.size())
        return QVariant();

    const auto &row = rowData[index.row()];

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch (index.column())
        {
            case 0: return row.name;
            case 1: return QString::number(row.readValue, 'f', 5);
            case 2: return QString::number(row.writeValue, 'f', 5);
            case 3: return "";
            case 4: return "";
            default: return QVariant();
        }
    }
    return QVariant();
}

bool ExTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid() || role != Qt::EditRole)
        return false;
    auto &row = rowData[index.row()];
    switch (index.column())
    {
        case 0:
            row.name = value.toString();
            break;
        case 2:
            row.writeValue = value.toDouble();
            break;
        default:
            return false;  // 序号和按钮列不允许编辑
    }
    emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
    return true;
}

Qt::ItemFlags ExTableModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;

    // 序号列和按钮列不可编辑
    if (index.column() == 1 || index.column() == 3 || index.column() == 4)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ExTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) return headers[section];

    if (orientation == Qt::Vertical) return QString::number(section + 1);
    return QVariant();
}

void ExTableModel::updateData(qreal runTime) {
    if (runTime-lastUpdateTime>(1000/targetFps) || runTime<lastUpdateTime) {
        lastUpdateTime = runTime;
        for (int i = 0; i < rowData.size(); ++i) {
            rowData[i].readValue = variContainer[i]->fValue;
        }
        emit dataChanged(index(0,1), index(rowData.size()-1,1), {Qt::DisplayRole, Qt::EditRole});
    }
}

void ExTableModel::appendRow(const QString &name,const QString&variName , const QString& type, std::size_t address, std::size_t size) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    rowData.emplace_back(TableVari{name, 0, 0});
    pushUnit(variName ,type, address, size);
    endInsertRows();
}

void ExTableModel::removeRow(int row) {
    if (rowData.isEmpty()) return;
    beginRemoveRows(QModelIndex(), row, row);
    rowData.remove(row);
    Backend::instance().sync.sendRequest([this,row]() {
        eraseUnit(row);
    });
    endRemoveRows();
}