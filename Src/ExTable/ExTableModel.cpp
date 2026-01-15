//
// Created by liaohy on 1/12/26.
//

#include "ExTableModel.h"

#include "Backend.hpp"
#include "VariComponent.hpp"

QStringList ExTableModel::headers{"name","read value", "write vaule",""};

ExTableModel::ExTableModel(QObject *parent) {
}

ExTableModel::~ExTableModel() {
    Backend::instance().sync.sendRequest([this]() {
        detachSelf();
    },Sync::Event::CLOSE_EVENT);
}

int ExTableModel::rowCount(const QModelIndex &parent) const {
    return rowData.size();
}

int ExTableModel::columnCount(const QModelIndex &parent) const {
    return headers.size();
}

QVariant ExTableModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= rowData.size())
        return QVariant();

    const auto &row = rowData[index.row()];

    if (role == Qt::DisplayRole)
    {
        switch (index.column())
        {
            case 0: return row.name;
            case 1: return QString::number(row.readValue, 'f', 5);
            case 2: return QString::number(row.writeValue, 'f', 5);
            case 3: return "";
            default: return QVariant();
        }
    }
    return QVariant();
}

bool ExTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid())
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
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    // // 序号列和按钮列不可编辑
    // if (index.column() == 3) return Qt::ItemIsEnabled;
    // if (index.column() == 2) return Qt::ItemIsEnabled|Qt::ItemIsEditable;
    // if (index.column() == 1)
    //     return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    //
    // return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant ExTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) return headers[section];

    if (orientation == Qt::Vertical) return QString::number(section + 1);
    return QVariant();
}

void ExTableModel::onPluginRunning(qreal runTime) {
    if (runTime-lastUpdateTime>(1000/targetFps) || runTime<lastUpdateTime) {
        lastUpdateTime = runTime;
        for (int i = 0; i < rowData.size(); ++i) {
            rowData[i].readValue = variContainer[i]->fValue;
        }
        QMetaObject::invokeMethod(this,[this]() {
            emit dataChanged(index(0,1), index(rowData.size()-1,1), {Qt::DisplayRole, Qt::EditRole});
        },Qt::QueuedConnection);
    }
}

void ExTableModel::appendRow(const QString &name,const QString&variName , const QString& type, std::size_t address, std::size_t size) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    auto vari = std::make_shared<VariComponent>(name,type,address,size);
    Backend::instance().sync.sendRequest([this, &vari, &name]() {
        rowData.emplace_back(TableVari{name, 0, 0});
        pushUnit(vari);
    });
    endInsertRows();
}

void ExTableModel::removeRow(int row) {
    if (rowData.isEmpty()) return;
    beginRemoveRows(QModelIndex(), row, row);
    qDebug()<<row;
    Backend::instance().sync.sendRequest([this,row]() {
        rowData.remove(row);
        eraseUnit(row);
    });
    endRemoveRows();
}

bool ExTableModel::sendWriteRequest(int row) {
    bool res = true;
    Backend::instance().sync.sendRequest([this,row,&res]() {
        auto& vari = variContainer[row];
        auto value = rowData[row].writeValue;
        res = Backend::instance().writeVari(vari.get(),value);
    },Sync::Event::WRITE_EVENT);
    return res;
}
