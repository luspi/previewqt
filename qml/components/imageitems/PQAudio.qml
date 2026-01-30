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

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtMultimedia
import PreviewQt

Item {

    id: audio_top

    width: PQCConstants.imageAvailableSize.width
    height: PQCConstants.imageAvailableSize.height

    property Item imageParent

    property var volumeList: [100, 80, 45, 0]
    property int volumeIndex: 0
    property var volumeIcon: ["high", "medium", "low", "mute"]

    property string musicTitle: ""
    property string musicArtist: ""
    property string musicAlbum: ""
    property string musicDate: ""
    property string musicTrack: ""

    Component.onCompleted: {
        PQCConstants.imageAsynchronous = true

        PQCConstants.mediainfoIsAudio = false
        PQCConstants.mediainfoIsVideo = false
        PQCConstants.mediainfoTitle = ""
        PQCConstants.mediainfoArtist = ""
        PQCConstants.mediainfoAlbum = ""
        PQCConstants.mediainfoDate = ""
        PQCConstants.mediainfoGenre = ""
        PQCConstants.mediainfoCopyright = ""
        PQCConstants.mediainfoAudioTrack = ""
        PQCConstants.mediainfoAudioBitRate = ""
        PQCConstants.mediainfoAudioCodec = ""
        PQCConstants.mediainfoVideoFrameRate = ""
        PQCConstants.mediainfoVideoBitRate = ""
        PQCConstants.mediainfoVideoCodec = ""
        PQCConstants.mediainfoVideoHdr = ""
    }

    Component.onDestruction: {
        PQCConstants.mainwindowOverrideTitle = ""

        PQCConstants.mediainfoIsAudio = false
        PQCConstants.mediainfoIsVideo = false
        PQCConstants.mediainfoTitle = ""
        PQCConstants.mediainfoArtist = ""
        PQCConstants.mediainfoAlbum = ""
        PQCConstants.mediainfoDate = ""
        PQCConstants.mediainfoGenre = ""
        PQCConstants.mediainfoCopyright = ""
        PQCConstants.mediainfoAudioTrack = ""
        PQCConstants.mediainfoAudioBitRate = ""
        PQCConstants.mediainfoAudioCodec = ""
        PQCConstants.mediainfoVideoFrameRate = ""
        PQCConstants.mediainfoVideoBitRate = ""
        PQCConstants.mediainfoVideoCodec = ""
        PQCConstants.mediainfoVideoHdr = ""
    }

    Image {
        id: musicImage
        property int dim: Math.min(Math.min(200, parent.width*(2/3)), Math.min(200, parent.height*(2/3)))
        x: (parent.width-width)/2
        y: (parent.height-height)/2
        width: dim
        height: dim
        sourceSize: Qt.size(width, height)
        visible: !coverImage.visible
        opacity: 0.8
        source: "image://svg/:/musicnote.svg"
    }

    Image {
        id: coverImage
        x: (parent.width-width)/2
        y: (parent.height-controls.height-height)/2
        width: Math.min(parent.width-20, parent.height-controls.height-20)
        height: width
        sourceSize: Qt.size(width, height)
        property string cover: ""
        visible: cover!==""
        source: (cover==="" ? "" : ("file://"+cover))
    }

    MediaPlayer {
        id: musicPlayer
        source: "file://"+PQCConstants.currentSource
        audioOutput: AudioOutput {
            volume: audio_top.volumeList[audio_top.volumeIndex]/100
        }
        onMediaStatusChanged: {
            if(mediaStatus == MediaPlayer.LoadedMedia) {
                PQCConstants.imageStatus = Image.Ready
                musicPlayer.play()
                PQCConstants.mediainfoIsAudio = true
            } else if(mediaStatus == MediaPlayer.InvalidMedia) {
                musicImage.source = "image://svg/:/errorimage.svg"
                PQCConstants.imageStatus = Image.Error
            }
        }

        onMetaDataChanged: {

            PQCConstants.mediainfoTitle = metaData.stringValue(MediaMetaData.Title)
            if(PQCConstants.mediainfoTitle === "") PQCConstants.mediainfoTitle = PQCScriptsFilesPaths.getBasename(PQCConstants.currentSource)

            PQCConstants.mediainfoArtist = metaData.stringValue(MediaMetaData.AlbumArtist)
            if(PQCConstants.mediainfoArtist === "") PQCConstants.mediainfoArtist = metaData.stringValue(MediaMetaData.ContributingArtist)

            if(PQCConstants.mediainfoTitle !== "" && PQCConstants.mediainfoArtist !== "")
                PQCConstants.mainwindowOverrideTitle = PQCConstants.mediainfoTitle + " | " + PQCConstants.mediainfoArtist
            else if(PQCConstants.mediainfoTitle !== "" || PQCConstants.mediainfoArtist !== "")
                PQCConstants.mainwindowOverrideTitle = PQCConstants.mediainfoTitle + PQCConstants.mediainfoArtist

            PQCConstants.mediainfoDate = metaData.stringValue(MediaMetaData.Date)
            PQCConstants.mediainfoAlbum = metaData.stringValue(MediaMetaData.AlbumTitle)
            PQCConstants.mediainfoAudioTrack = metaData.stringValue(MediaMetaData.TrackNumber)
            PQCConstants.mediainfoGenre = metaData.stringValue(MediaMetaData.Genre)
            PQCConstants.mediainfoCopyright = metaData.stringValue(MediaMetaData.Copyright)
            PQCConstants.mediainfoAudioTrack = metaData.stringValue(MediaMetaData.TrackNumber)
            PQCConstants.mediainfoAudioBitRate = metaData.stringValue(MediaMetaData.AudioBitRate)
            PQCConstants.mediainfoAudioCodec = metaData.stringValue(MediaMetaData.AudioCodec)

            var img = metaData.value(MediaMetaData.CoverArtImage)
            if(img === undefined) img = metaData.value(MediaMetaData.ThumbnailImage)
            if(img === undefined) {

                console.log("no embedded cover image found, looing for cover file next to music file")

                // check for existence of cover file next to file
                img = PQCScriptsImages.findCoverImageNextToFile(PQCConstants.currentSource)
                console.log("found separate cover image:", img)
                coverImage.cover = img

            } else {

                console.log("found embedded cover file, attempting to save and display it")

                var path = PQCScriptsFilesPaths.saveImageToTempFile(img)
                coverImage.cover = path

            }

        }

    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if(musicPlayer.playbackState === MediaPlayer.PlayingState)
                musicPlayer.pause()
            else
                musicPlayer.play()
        }
        onDoubleClicked: (mouse) => {
            if(mouse.button === Qt.RightButton) return
            if(PQCConstants.mainwindowIsFullscreen)
                PQCNotify.mainwindowShowNormal()
            else
                PQCNotify.mainwindowShowFullscreen()
        }
    }

    Rectangle {

        id: controls

        y: parent.height-height
        width: parent.width
        height: 50

        color: "#88000000"

        MouseArea {
            id: controlsmouse
            anchors.fill: parent
            hoverEnabled: true
        }

        RowLayout {
            id: controlrow
            x: 5
            y: (parent.height-height)/2
            width: parent.width-10
            spacing: 5
            Image {
                id: playpausebutton
                y: (parent.height-height)/2
                width: height
                height: 20
                sourceSize: Qt.size(width, height)
                source: musicPlayer.playbackState===MediaPlayer.PlayingState ? "image://svg/:/pause.svg" : "image://svg/:/play.svg"
                MouseArea {
                    id: playpausemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        if(musicPlayer.playbackState === MediaPlayer.PlayingState)
                            musicPlayer.pause()
                        else
                            musicPlayer.play()
                    }
                }
            }
            Text {
                id: sliderlefttxt
                y: (parent.height-height)/2
                color: "white"
                text: PQCScriptsOther.convertSecondsToHumandFriendly(Math.round(musicPlayer.position/1000), Math.round(musicPlayer.duration/1000))
            }

            Slider {
                id: slider
                y: (parent.height-height)/2
                orientation: Qt.Horizontal
                Layout.fillWidth: true
                from: 0
                value: Math.round(musicPlayer.position/1000)
                to: Math.round(musicPlayer.duration/1000)
                onValueChanged: {
                    if(pressed) {
                        value = value
                        musicPlayer.setPosition(value*1000)
                        value = Qt.binding(function() { return Math.round(musicPlayer.position/1000) })
                    }
                    PQCNotify.resetFocus()
                }
            }
            Text {
                id: sliderrighttxt
                y: (parent.height-height)/2
                color: "white"
                text: PQCScriptsOther.convertSecondsToHumandFriendly(Math.round(musicPlayer.duration/1000))
            }
            Image {
                id: volumebutton
                y: (parent.height-height)/2
                width: height
                height: 20
                sourceSize: Qt.size(width, height)
                source: "image://svg/:/volume_" + audio_top.volumeIcon[audio_top.volumeIndex] + ".svg"
                MouseArea {
                    id: volumemouse
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        audio_top.volumeIndex = (volumeIndex+1)%4
                    }
                }
            }


        }

    }

}
