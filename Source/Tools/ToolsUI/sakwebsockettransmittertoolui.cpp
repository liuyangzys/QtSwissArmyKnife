﻿/***************************************************************************************************
 * Copyright 2023 x-tools-author(x-tools@outlook.com). All rights reserved.
 *
 * The file is encoded using "utf8 with bom", it is a part of xTools project.
 *
 * xTools is licensed according to the terms in the file LICENCE(GPL V3) in the root of the source
 * code directory.
 **************************************************************************************************/
#include "sakwebsockettransmittertoolui.h"

SAKWebSocketTransmitterToolUi::SAKWebSocketTransmitterToolUi(QWidget *parent)
    : SAKSocketClientTransmitterToolUi("SAK.WebSocketTransmitterToolUi", parent)
{}

void SAKWebSocketTransmitterToolUi::onBaseToolUiInitialized(SAKBaseTool *tool,
                                                            const QString &settingGroup)
{
    SAKSocketClientTransmitterToolUi::onBaseToolUiInitialized(tool, settingGroup);

    setColumnVisible(4, true);
    mEditor->setWebSocketContextVisible(true);
}