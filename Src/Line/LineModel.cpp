//
// Created by liaohy on 12/25/25.
//


#include "LineModel.hpp"

#include "LineGroupModel.hpp"

LineModel::LineModel(QObject *parent) : QAbstractItemModel(parent) {
}

LineModel::~LineModel() = default;

int LineModel::rowCount(const QModelIndex &parent) const {
    return lines.size();
}

QVariant LineModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    auto line = lines.at(index.row());
    switch (role) {
        case NameRole:
            return line->getLineName();
        case ColorRole:
            return line->getLineColor();
        case VisibleRole:
            return line->isVisible();
        case DataLenRole:
            return static_cast<qint64>(line->getLen());
        case BufCapRole:
            return static_cast<qint64>(line->getCapacity());
        case ObjectRole:
            return QVariant::fromValue(line.get());
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> LineModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ColorRole] = "color";
    roles[VisibleRole] = "visible";
    roles[DataLenRole] = "len";
    roles[BufCapRole] = "cap";
    roles[ObjectRole] = "object";
    return roles;
}

QModelIndex LineModel::index(int row, int column, const QModelIndex &parent) const {
    if (row<0||row>=rowCount(parent)||column!=0)
        return QModelIndex();
    return createIndex(row,0);
}

QModelIndex LineModel::parent(const QModelIndex &child) const {
    return QModelIndex();
}

int LineModel::columnCount(const QModelIndex &parent) const {
    return 1;
}

bool LineModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (!index.isValid()||index.row()>=lines.size()) return false;
    auto line = lines[index.row()];
    bool changed = false;
    bool update = false;
    switch (role) {
        case NameRole:
        if (line->getLineName()!=value.toString()) {
            line->setLineName(value.toString());
            changed = true;
        }
            break;
        case ColorRole:
            if (line->getLineColor()!=value.toString()) {
                line->setColor(value.toString());
                changed = true;
                update = true;
            }
                break;
        case VisibleRole:
            if (line->isVisible()!=value.toBool()) {
                line->setVisible(value.toBool());
                changed = true;
            }
                break;
        case DataLenRole:
            return false;
        case BufCapRole:
            if (line->getCapacity()!=value.toLongLong()) {
                line->setCapacity(value.toLongLong());
                changed = true;
            }
                break;
        default:
            return false;
    }
    if (changed)
        emit dataChanged(index,index,{role});
    if (update)
        line->update();
    return changed;
}

Qt::ItemFlags LineModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

void LineModel::appendLine(const QString &name, const QColor& color, std::size_t bufferCapacity) {
    auto t = std::make_shared<ChartLine>(name, color, bufferCapacity);
    int row = lines.size();
    beginInsertRows(QModelIndex(),row,row);
    lines.emplace_back(t);
    endInsertRows();
    emit LineGroupModel::instance().lineCountChanged();
    emit lineCountChanged();
}

void LineModel::removeLine(int index) {
    beginRemoveRows(QModelIndex(),index,index);
    lines.remove(index);
    endRemoveRows();
    emit LineGroupModel::instance().lineCountChanged();
    emit lineCountChanged();
}

void LineModel::clear() {
    auto before = lines.size();
    beginResetModel();
    lines.clear();
    endResetModel();
    if (before!=lines.size()) {
        emit LineGroupModel::instance().lineCountChanged();
        emit lineCountChanged();
    }
}

bool LineModel::isAllVisible() {
    for (const auto & line: lines) if (!line->isVisible()) return false;
    return true;
}

void LineModel::setAllVisible(bool visible) {
    for (const auto & line: lines) {
        line->setVisible(visible);
    }
}
