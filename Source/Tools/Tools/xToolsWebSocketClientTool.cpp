﻿/***************************************************************************************************
 * Copyright 2023-2024 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "xToolsWebSocketClientTool.h"

#include "xToolsCompatibility.h"
#include "xToolsInterface.h"

xToolsWebSocketClientTool::xToolsWebSocketClientTool(QObject *parent)
    : xToolsSocketClientTool{parent}
{}

bool xToolsWebSocketClientTool::initialize(QString &errStr)
{
    Q_UNUSED(errStr)
    m_webSocket = new QWebSocket();
    connect(m_webSocket, &QWebSocket::connected, m_webSocket, [=]() {
        QString address = m_webSocket->localAddress().toString();
        QString port = QString::number(m_webSocket->localPort());
        this->m_bindingIpPort = address + ":" + port;
        emit bindingIpPortChanged();

        QString ip = m_webSocket->peerAddress().toString();
        this->m_peerInfo = ip + ":" + QString::number(m_webSocket->peerPort());
    });

    connect(m_webSocket, &QWebSocket::disconnected, m_webSocket, [=]() {
        QString errStr = m_webSocket->errorString();
        if (!errStr.isEmpty()) {
            errStr = "Disconected from server:" + errStr;
            emit errorOccurred(errStr);
        }

        exit();
    });

    connect(m_webSocket, &QWebSocket::binaryFrameReceived, m_webSocket, [=](const QByteArray &msg) {
        QByteArray ba = xToolsInterface::arrayToHex(msg, ' ');
        QString hex = QString::fromLatin1(ba);
        QString info = m_bindingIpPort + "<-" + this->m_peerInfo + ":" + hex;
        qInfo() << info;
        emit outputBytes(msg);
        emit bytesRead(msg, this->m_peerInfo);
    });

    connect(m_webSocket, &QWebSocket::textMessageReceived, m_webSocket, [=](QString message) {
        QString info = m_bindingIpPort + "<-" + this->m_peerInfo + ":" + message;
        qInfo() << info;
        emit outputBytes(message.toUtf8());
        emit bytesRead(message.toUtf8(), this->m_peerInfo);
    });

    connect(m_webSocket, SAK_SIG_WEBSOCKETERROROCCURRED, m_webSocket, [=]() {
        QString errStr = m_webSocket->errorString();
        emit errorOccurred(errStr);
    });
    
    QString address = "ws://" + m_serverIp + ":" + QString::number(m_serverPort);
    qDebug() << "Server url: " + address;
    m_webSocket->open(address);

    return true;
}

void xToolsWebSocketClientTool::writeBytes(const QByteArray &bytes)
{
    qint64 ret = -1;
    QString hex;
    if (m_messageType == 0) {
        hex = QString::fromLatin1(xToolsInterface::arrayToHex(bytes, ' '));
        ret = m_webSocket->sendBinaryMessage(bytes);
    } else {
        hex = QString::fromUtf8(bytes);
        ret = m_webSocket->sendTextMessage(QString::fromUtf8(bytes));
    }

    if (ret == -1) {
        qWarning() << "Send message error: " + m_webSocket->errorString();
        emit errorOccurred(m_webSocket->errorString());
    } else {
        QString info = m_bindingIpPort + "<-" + this->m_peerInfo + ":" + hex;
        qInfo() << info;
        emit bytesWritten(bytes, this->m_peerInfo);
    }
}

void xToolsWebSocketClientTool::uninitialize()
{
    m_webSocket->close();
    m_webSocket->deleteLater();
    m_webSocket = nullptr;
}
