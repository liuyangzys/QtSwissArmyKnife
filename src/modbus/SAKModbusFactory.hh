/******************************************************************************
 * Copyright 2023 Qsaker(qsaker@foxmail.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 *****************************************************************************/
#ifndef SAKMODBUSFACTORY_HH
#define SAKMODBUSFACTORY_HH

#include <QObject>
#include <QLoggingCategory>

class SAKModbusFactory : public QObject
{
    Q_OBJECT
public:
    enum ModbusDeviceType {
        ModbusRtuSerialClient,
        ModbusRtuSerialServer,
        ModbusTcpClient,
        ModbusTcpServer
    };
    Q_ENUM(ModbusDeviceType)

public:
    SAKModbusFactory(QObject *parent = Q_NULLPTR);
    ~SAKModbusFactory();

    const QString typeName(int type);

private:
    const QLoggingCategory mLoggingCategory{"sak.modbus.factory"};
};

#endif // SAKMODBUSFACTORY_HH