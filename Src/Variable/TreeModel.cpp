//
// Created by liaohy on 12/19/25.
//


#include "TreeModel.hpp"

#include <qurl.h>

#include "GetVariTree.hpp"

TreeModel::TreeModel(const std::shared_ptr<VariTree> &data, QObject *parent)
    : QAbstractItemModel(parent),
      tree(data) {
}

TreeModel::~TreeModel() {
}

QHash<int, QByteArray> TreeModel::roleNames() const {
    return {
        {Qt::DisplayRole, "display"},
        {TypeRole, "type"},
        {SizeRole, "size"},
        {AddrRole, "addr"},
    };
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    auto item = static_cast<VariNode *>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        return QString::fromStdString(item->data(index.column()));
    } else if (role == TypeRole) {
        return item->getType().data();
    } else if (role == AddrRole) {
        return static_cast<qulonglong>(item->getAbsolute());
    } else if (role == SizeRole) {
        return static_cast<qulonglong>(item->getSize());
    }
    return QVariant();
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return QString("Name");
            case 1:
                return QString("Type");
            case 2:
                return QString("Address");
            case 3:
                return QString("Size");

        }
    }
    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return QModelIndex();
    VariNode *parentItem = parent.isValid()
                               ? static_cast<VariNode *>(parent.internalPointer())
                               : tree.get();
    VariNode *childItem = parentItem->getChild(row);
    if (childItem) return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();
    auto *childItem = static_cast<VariNode *>(index.internalPointer());
    VariNode *parentItem = childItem->getParent();

    return parentItem != tree.get()
               ? createIndex(parentItem->getIndex(), 0, parentItem)
               : QModelIndex{};
}

int TreeModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) { return tree->getChildSize(); }
    auto parent_node = static_cast<VariNode *>(parent.internalPointer());
    return parent_node->getChildSize();
}

int TreeModel::columnCount(const QModelIndex &parent) const {
    return 4;
}

void TreeModel::setTreeData(const QUrl& path) {
    beginResetModel();
    qDebug()<<path.path().toStdString();
    tree = getVariTree(path.path().toStdString());
    endResetModel();
}

QModelIndex TreeModel::findNode(const QString &nodeName) {
    auto nodesName = nodeName.split('.');
    qDebug()<<nodesName;
    if (nodesName.isEmpty()) return QModelIndex();
    VariNode* node = nullptr;
    QModelIndex mIndex;
    for (int i = 0; i < tree->getChildSize(); ++i) {
        node = tree->getChild(i)->findChild(nodesName[0].toStdString());
        if (node) {
            mIndex = index(i,0);
            mIndex = index(node->getIndex(),0,mIndex);
            break;
        }
    }
    if (!node) return QModelIndex();
    qDebug()<<data(mIndex,Qt::DisplayRole);
    for (int i = 1; i < nodesName.size(); ++i) {
        node =  node->findChild(nodesName[i].toStdString());
        if (node) mIndex =  index(node->getIndex(),0,mIndex); else return QModelIndex();
    }
    qDebug()<<data(mIndex,Qt::DisplayRole);
    return mIndex;
}
