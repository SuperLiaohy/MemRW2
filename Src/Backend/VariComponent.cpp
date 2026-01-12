//
// Created by liaohy on 1/5/26.
//

#include "VariComponent.hpp"
#include "DisplayPluginInterface.hpp"
#include <QDebug>

VariComponent::VariComponent(const QString&name , const QString& type, std::size_t address, std::size_t size)  : active(true),
name(name), size(size), address(address) {
    this->type = getTypeFromString(type);
    qDebug()<<"create: "<<"name: "<<name << "size: "<<size<<"type: "<<getTypeName()<<"address: "<<address;
}

VariComponent::~VariComponent() {
    qDebug()<<"delete: "<<"name: "<<name << "size: "<<size<<"type: "<<getTypeName()<<"address: "<<address;
}

void VariComponent::updateFromModel(const QModelIndex &index) {
    auto variNode = static_cast<VariNode *>(index.internalPointer());
    size = variNode->getSize();
    type = getTypeFromString(QString::fromStdString(variNode->getType()));
    address = variNode->getAbsolute();
}

QString VariComponent::getTypeName() const {
    switch (type) {
        case Type::INT8:
            return QString("int8_t");
        case Type::UINT8:
            return QString("uint8_t");
        case Type::INT16:
            return QString("int16_t");
        case Type::UINT16:
            return QString("uint16_t");
        case Type::INT32:
            return QString("int32_t");
        case Type::UINT32:
            return QString("uint32_t");
        case Type::INT64:
            return QString("int64_t");
        case Type::UINT64:
            return QString("uint64_t");
        case Type::FLOAT:
            return QString("float");
        case Type::DOUBLE:
            return QString("double");
    }
}

VariComponent::Type VariComponent::getTypeFromString(const QString &type_name) {
    if (type_name.contains("uint8_t"))
        return Type::UINT8;
    if (type_name.contains("uint16_t"))
        return Type::UINT16;
    if (type_name.contains("uint32_t"))
        return Type::UINT32;
    if (type_name.contains("uint64_t"))
        return Type::UINT64;
    if (type_name.contains("int8_t"))
        return Type::INT8;
    if (type_name.contains("int16_t"))
        return Type::INT16;
    if (type_name.contains("int32_t"))
        return Type::INT32;
    if (type_name.contains("int64_t"))
        return Type::INT64;
    if (type_name.contains("float"))
        return Type::FLOAT;
    if (type_name.contains("double"))
        return Type::DOUBLE;
    if (type_name.contains("f")) {
        if (type_name.contains("32")&&size==4)
            return Type::FLOAT;
        if (type_name.contains("64")&&size==8)
            return Type::DOUBLE;
    }
    if (type_name.contains("unsigned")) {
        if (type_name.contains("char")&&size==1)
            return Type::UINT8;
        if (type_name.contains("short")&&size==2)
            return Type::UINT16;
        if (type_name.contains("int")&&size==4)
            return Type::UINT32;
        if (type_name.contains("long")&&type_name.indexOf("long")!=type_name.lastIndexOf("long")&&size==8)
            return Type::UINT64;
        return Type::UINT32;
    } else {
        if (type_name.contains("char")&&size==1)
            return Type::INT8;
        if (type_name.contains("short")&&size==2)
            return Type::INT16;
        if (type_name.contains("int")&&size==4)
            return Type::INT32;
        if (type_name.contains("long")&&type_name.indexOf("long")!=type_name.lastIndexOf("long")&&size==8)
            return Type::INT64;
        return Type::INT32;
    }
}
