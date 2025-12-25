//
// Created by liaohy on 12/25/25.
//

#pragma once

#include <QAbstractItemModel>
#include "ChartLine.hpp"

class LineModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    Q_DISABLE_COPY_MOVE(LineModel)
    enum RoleNames {
        NameRole = Qt::UserRole + 1,
        ColorRole,
        VisibleRole,
        DataLenRole,
        BufCapRole,
    };

    explicit LineModel(QObject *parent = nullptr);
    ~LineModel();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int columnCount(const QModelIndex &parent) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_INVOKABLE void append(const QString& name,const QColor& color);
    Q_INVOKABLE void remove(int index);
    Q_INVOKABLE void clear();
private:
    QVector<std::shared_ptr<ChartLine>> lines;

};