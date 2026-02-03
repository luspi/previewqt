/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2026 Lukas Spies                                       **
 ** Contact: https://previewqt.org                                       **
 **                                                                      **
 ** This file is part of PreviewQt.                                      **
 **                                                                      **
 ** PreviewQt is free software: you can redistribute it and/or modify    **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PreviewQt is distributed in the hope that it will be useful,         **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PreviewQt. If not, see <http://www.gnu.org/licenses/>.    **
 **                                                                      **
 **************************************************************************/

// This file is included in main.cpp
// and this function is installed as message handler
#ifdef NDEBUG
#include <pqc_scriptsconfig.h>
#endif

void pqcMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg) {

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    const QDateTime date = QDateTime::currentDateTime();
    const QFileInfo fileinfo(file);
    QByteArray filename = fileinfo.fileName().toLatin1();
    switch (type) {
    case QtDebugMsg:
#ifdef NDEBUG
    if(PQCScriptsConfig::get().isDebug()) {
#endif
        fprintf(stderr, "%s [D] %s::%s::%u: %s\n", date.toString("yyyy-MM-dd HH:mm:ss.zzz").toLatin1().constData(), filename.constData(), function, context.line, msg.toLocal8Bit().constData());
#ifdef NDEBUG
    }
#endif
        break;
    case QtInfoMsg:
        fprintf(stderr, "%s [I] %s::%s::%u: %s\n", date.toString("yyyy-MM-dd HH:mm:ss.zzz").toLatin1().constData(), filename.constData(), function, context.line, msg.toLocal8Bit().constData());
        break;
    case QtWarningMsg:
        fprintf(stderr, "%s [W] %s::%s::%u: %s\n", date.toString("yyyy-MM-dd HH:mm:ss.zzz").toLatin1().constData(), filename.constData(), function, context.line, msg.toLocal8Bit().constData());
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s [C] %s::%s::%u: %s\n", date.toString("yyyy-MM-dd HH:mm:ss.zzz").toLatin1().constData(), filename.constData(), function, context.line, msg.toLocal8Bit().constData());
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s [F] %s::%s::%u: %s\n", date.toString("yyyy-MM-dd HH:mm:ss.zzz").toLatin1().constData(), filename.constData(), function, context.line, msg.toLocal8Bit().constData());
        std::exit(1);
        break;
    }
}
