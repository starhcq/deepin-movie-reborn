// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QLibrary>
#include <QDir>
#include <QLibraryInfo>
#include <QJsonDocument>

#include "eventlogutils.h"
#include "compositing_manager.h"

EventLogUtils *EventLogUtils::m_instance(nullptr);

EventLogUtils &EventLogUtils::get()
{
    if (m_instance == nullptr) {
        m_instance = new EventLogUtils;
    }
    return *m_instance;
}

EventLogUtils::EventLogUtils()
{
    QLibrary library(dmr::CompositingManager::libPath("libdeepin-event-log.so"));

    init =reinterpret_cast<bool (*)(const std::string &, bool)>(library.resolve("Initialize"));
    writeEventLog = reinterpret_cast<void (*)(const std::string &)>(library.resolve("WriteEventLog"));

    if (init == nullptr)
        return;

    init("deepin-movie", true);
}

void EventLogUtils::writeLogs(QJsonObject &data)
{
    if (writeEventLog == nullptr)
        return;

    //std::string str = QJsonDocument(data).toJson(QJsonDocument::Compact).toStdString();
    writeEventLog(QJsonDocument(data).toJson(QJsonDocument::Compact).toStdString());
}
