//
// Created by liaohy on 12/27/25.
//

#pragma once

#include <QAbstractListModel>
#include <QColor>

class ExChartView;

enum BasicType {
    UINT8_T,
    UINT16_T,
    UINT32_T,
    INT8_T,
    INT16_T,
    INT32_T,
    FLOAT_T,

    UINT64_T,
    INT64_T,
    DOUBLE_T,
};


struct LineAttr {
    QString group;
    // qml change
    struct {
        /***** paint *****/
        QColor color;
        QString name;
        bool visible;
        /***** buffer *****/
        quint32 capacity;
    } config;
    // cpp change
    struct {
        /***** paint *****/
        quint32 paintCount;
        /***** buffer *****/
        quint32 pointsLen;
    } view;
};

class LineAttrModel : public QAbstractListModel {
    Q_OBJECT
public:
    Q_DISABLE_COPY_MOVE(LineAttrModel)

    enum RoleNames {
        NameRole = Qt::UserRole + 1,
        ColorRole,
        VisibleRole,
        BufCapRole,
        BufLenRole,
        PaintLenRole,
        GroupRole,
        VariName,
        VariType,
        VariAddr,
        VariSize,
    };
    explicit LineAttrModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}
    LineAttrModel()=default;
    ~LineAttrModel()=default;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_INVOKABLE void appendLine(const QString& name, const QColor& color, quint32 bufferCapacity, const QString&variName , const QString& type, std::size_t address, std::size_t size);
    Q_INVOKABLE void removeLine(int index);
    friend ExChartView;
private:
    QVector<std::shared_ptr<LineAttr>> lineAttrs;
};