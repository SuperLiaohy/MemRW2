//
// Created by liaohy on 1/12/26.
//

#pragma once

#include <QAbstractTableModel>
#include <QColor>
#include <qqmlintegration.h>

#include "DisplayPluginInterface.hpp"

struct TableVari {
    /***** read *****/
    QString name;
    /***** read *****/
    qreal readValue;
    /***** write *****/
    qreal writeValue;
};

class ExTableModel: public QAbstractTableModel, public DisplayPluginInterface
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(quint32 targetFps READ getTargetFps WRITE setTargetFps NOTIFY targetFpsChanged)

public:
    explicit ExTableModel(QObject *parent = nullptr);
    ~ExTableModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void onPluginRunning(qreal runTime) override;
    bool onGeneratePluginSetting(QTextStream &stream) override;
    bool onLoadPluginSetting(QTextStream &stream) override;
    Q_INVOKABLE void appendRow(const QString &name,const QString&variName , const QString& type, std::size_t address, std::size_t size);
    Q_INVOKABLE void removeRow(int row);
    Q_INVOKABLE bool sendWriteRequest(int row);


    [[nodiscard]] quint32 getTargetFps() const{ return targetFps;}
    void setTargetFps(quint32 fps){ targetFps = fps; emit targetFpsChanged();}
signals:
    void targetFpsChanged();

private:
    qreal lastUpdateTime = 0;
    quint32 targetFps = 30;

    QVector <TableVari> rowData;
    static QStringList headers;
};
