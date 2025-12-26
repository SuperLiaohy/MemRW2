//
// Created by liaohy on 12/25/25.
//

#pragma once

#include <QAbstractItemModel>
#include "ChartLine.hpp"

class LineModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged)

public:
    Q_DISABLE_COPY_MOVE(LineModel)
    enum RoleNames {
        NameRole = Qt::UserRole + 1,
        ColorRole,
        VisibleRole,
        DataLenRole,
        BufCapRole,
        ObjectRole,
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

    Q_INVOKABLE void appendLine(const QString& name, const QColor& color, std::size_t bufferCapacity = 5000);
    Q_INVOKABLE void removeLine(int index);
    Q_INVOKABLE void clear();
    Q_INVOKABLE std::size_t lineCount() {return lines.count();}

    bool isAllVisible();
    void setAllVisible(bool visible);
    std::shared_ptr<ChartLine>& operator[](std::size_t index) {return lines[index];}

public: signals:
    void lineCountChanged();

private:
    QVector<std::shared_ptr<ChartLine>> lines;
};