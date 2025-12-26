//
// Created by liaohy on 12/25/25.
//

#pragma once

#include <QAbstractListModel>
#include <qqmlengine.h>

#include "LineModel.hpp"

class LineGroupModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_DISABLE_COPY_MOVE(LineGroupModel)

    Q_PROPERTY(int groupCount READ groupCount NOTIFY groupCountChanged)
    Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged)
    LineGroupModel(QObject* parent = nullptr);

public:
    static LineGroupModel& instance() {
        static LineGroupModel manager;
        return manager;
    }
    static LineGroupModel* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine) {
        Q_UNUSED(jsEngine)Q_UNUSED(qmlEngine)
        qDebug()<<"qml get cpp static instance.";
        auto result = &instance();
        QQmlEngine::setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
        return result;
    }

    enum RoleNames {
        NameRole = Qt::UserRole + 1,
        VisibleRole,
        SelectedRole,
        ChildrenRole,
    };

    ~LineGroupModel();

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    Q_INVOKABLE bool appendGroup(const QString& groupName);
    Q_INVOKABLE void removeGroup(int index);

    Q_INVOKABLE std::size_t groupCount() {return groups.size();}
    Q_INVOKABLE std::size_t lineCount() {
        std::size_t count = 0;
        for (const auto & group: groups) {
            count+=group.lineModel->lineCount();
        }
        return count;
    }

public: signals:
    void groupCountChanged();
    void lineCountChanged();
public:
    struct Group {
        Group(const QString& n) : lineModel(std::make_shared<LineModel>()), name(n) {}
        std::shared_ptr<LineModel> lineModel;
        QString name;
    };

    std::shared_ptr<LineModel>& operator[](std::size_t index) {return groups[index].lineModel;}

private:
    QVector<Group> groups;
};