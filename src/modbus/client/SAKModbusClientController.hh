﻿/*
 * Copyright 2020 Qter(qsaker@qq.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part
 * of QtSwissArmyKnife project.
 *
 * QtSwissArmyKnife is licensed according to the terms in
 * the file LICENCE in the root of the source code directory.
 */
#ifndef SAKMODBUSCLIENTCONTROLLER_HH
#define SAKMODBUSCLIENTCONTROLLER_HH

#include <QModbusClient>

#include "SAKModbusCommonController.hh"

class SAKModbusCommonClientSection;
class SAKModbusClientController : public SAKModbusCommonController
{
public:
    SAKModbusClientController(QWidget *parent = Q_NULLPTR);
protected:
    SAKModbusCommonClientSection *mClientSection;
protected:
    virtual QWidget *bottomSection() final;
    void sendReadRequest(QModbusDataUnit mdu);
    void sendWriteRequest(QModbusDataUnit mdu);
private:
    void readReply();
};

#endif // SAKMODBUSCLIENTCONTROLLER_HH