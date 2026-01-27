/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2025 Lukas Spies                                       **
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
#pragma once

#include <QObject>
#include <QQmlEngine>

class QSettings;
class QTimer;

class PQCSettings : public QObject {

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    explicit PQCSettings();
    ~PQCSettings();

    Q_PROPERTY(QString version MEMBER m_version NOTIFY versionChanged)
    Q_PROPERTY(QString language MEMBER m_language NOTIFY languageChanged)
    Q_PROPERTY(bool topBarAutoHide MEMBER m_topBarAutoHide NOTIFY topBarAutoHideChanged)
    Q_PROPERTY(bool hideToSystemTray MEMBER m_hideToSystemTray NOTIFY hideToSystemTrayChanged)
    Q_PROPERTY(bool launchHiddenToSystemTray MEMBER m_launchHiddenToSystemTray NOTIFY launchHiddenToSystemTrayChanged)
    Q_PROPERTY(bool notifyNextLaunchHiddenToSystemTray MEMBER m_notifyNextLaunchHiddenToSystemTray NOTIFY notifyNextlaunchHiddenToSystemTrayChanged)
    Q_PROPERTY(bool maximizeImageSizeAndAdjustWindow MEMBER m_maximizeImageSizeAndAdjustWindow NOTIFY maximizeImageSizeAndAdjustWindowChanged)
    Q_PROPERTY(int defaultWindowWidth MEMBER m_defaultWindowWidth NOTIFY defaultWindowWidthChanged)
    Q_PROPERTY(int defaultWindowHeight MEMBER m_defaultWindowHeight NOTIFY defaultWindowHeightChanged)
    Q_PROPERTY(bool defaultWindowMaximized MEMBER m_defaultWindowMaximized NOTIFY defaultWindowMaximizedChanged)
    Q_PROPERTY(QString defaultAppShortcut MEMBER m_defaultAppShortcut NOTIFY defaultAppShortcutChanged)
    Q_PROPERTY(QString defaultAppImages MEMBER m_defaultAppImages NOTIFY defaultAppImagesChanged)
    Q_PROPERTY(QString defaultAppDocuments MEMBER m_defaultAppDocuments NOTIFY defaultAppDocumentsChanged)
    Q_PROPERTY(QString defaultAppArchives MEMBER m_defaultAppArchives NOTIFY defaultAppArchivesChanged)
    Q_PROPERTY(QString defaultAppVideos MEMBER m_defaultAppVideos NOTIFY defaultAppVideosChanged)
    Q_PROPERTY(QString defaultAppComicBooks MEMBER m_defaultAppComicBooks NOTIFY defaultAppComicBooksChanged)
    Q_PROPERTY(QString defaultAppEBooks MEMBER m_defaultAppEBooks NOTIFY defaultAppEBooksChanged)
    Q_PROPERTY(QString defaultAppText MEMBER m_defaultAppText NOTIFY defaultAppTextChanged)
    Q_PROPERTY(QString defaultAppUrl MEMBER m_defaultAppUrl NOTIFY defaultAppUrlChanged)
    Q_PROPERTY(bool closeAfterDefaultApp MEMBER m_closeAfterDefaultApp NOTIFY closeAfterDefaultAppChanged)
    Q_PROPERTY(QString filedialogLocation MEMBER m_filedialogLocation NOTIFY filedialogLocationChanged)
    Q_PROPERTY(bool closeWhenLosingFocus MEMBER m_closeWhenLosingFocus NOTIFY closeWhenLosingFocusChanged)
    Q_PROPERTY(bool textWordWrap MEMBER m_textWordWrap NOTIFY textWordWrapChanged)
    Q_PROPERTY(int textFontPointSize MEMBER m_textFontPointSize NOTIFY textFontPointSizeChanged)
    Q_PROPERTY(bool textSearchCaseSensitive MEMBER m_textSearchCaseSensitive NOTIFY textSearchCaseSensitiveChanged)

    Q_INVOKABLE bool getFirstStart() { return firstStart; }

private:
    QString m_version;
    QString m_language;
    bool m_topBarAutoHide;
    bool m_hideToSystemTray;
    bool m_launchHiddenToSystemTray;
    bool m_notifyNextLaunchHiddenToSystemTray;
    bool m_maximizeImageSizeAndAdjustWindow;
    int m_defaultWindowWidth;
    int m_defaultWindowHeight;
    bool m_defaultWindowMaximized;
    QString m_defaultAppShortcut;
    QString m_defaultAppImages;
    QString m_defaultAppDocuments;
    QString m_defaultAppArchives;
    QString m_defaultAppVideos;
    QString m_defaultAppComicBooks;
    QString m_defaultAppEBooks;
    QString m_defaultAppText;
    QString m_defaultAppUrl;
    bool m_closeAfterDefaultApp;
    QString m_filedialogLocation;
    bool m_closeWhenLosingFocus;
    bool m_textWordWrap;
    int m_textFontPointSize;
    bool m_textSearchCaseSensitive;

    QSettings *settings;
    QTimer *saveTimer;

    void loadSettings();

    bool checkToolExistence(QString tool);

    bool firstStart;

    QStringList opt_img;
    QStringList opt_doc;
    QStringList opt_arc;
    QStringList opt_com;
    QStringList opt_bok;
    QStringList opt_vid;
    QStringList opt_txt;
    QStringList opt_url;

private Q_SLOTS:
    void saveSettings();

Q_SIGNALS:
    void versionChanged();
    void languageChanged();
    void topBarAutoHideChanged();
    void hideToSystemTrayChanged();
    void launchHiddenToSystemTrayChanged();
    void notifyNextlaunchHiddenToSystemTrayChanged();
    void maximizeImageSizeAndAdjustWindowChanged();
    void defaultWindowWidthChanged();
    void defaultWindowHeightChanged();
    void defaultWindowMaximizedChanged();
    void defaultAppShortcutChanged();
    void defaultAppImagesChanged();
    void defaultAppDocumentsChanged();
    void defaultAppArchivesChanged();
    void defaultAppVideosChanged();
    void defaultAppComicBooksChanged();
    void defaultAppEBooksChanged();
    void defaultAppTextChanged();
    void defaultAppUrlChanged();
    void closeAfterDefaultAppChanged();
    void filedialogLocationChanged();
    void closeWhenLosingFocusChanged();
    void textWordWrapChanged();
    void textFontPointSizeChanged();
    void textSearchCaseSensitiveChanged();

};
