//
// Created by liaohy on 12/19/25.
//

#pragma once

#include <QAbstractItemModel>
#include "VariTree.hpp"

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    Q_DISABLE_COPY_MOVE(TreeModel)
    enum RoleNames {
        NameRole = Qt::UserRole + 1,
        TypeRole,
        SizeRole,
        AddrRole,
    };
    explicit TreeModel(const std::shared_ptr<VariTree> &header, QObject *parent = nullptr);
    ~TreeModel() override;

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role) const override;
    // Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = {}) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = {}) const override;
    int columnCount(const QModelIndex &parent = {}) const override;

    Q_INVOKABLE void setTreeData(const QUrl& path);
    Q_INVOKABLE QModelIndex findNode(const QString& nodeName);
private:
    std::shared_ptr<VariTree> tree;
};