/**************************************************************************
 **                                                                      **
 ** Copyright (C) 2011-2026 Lukas Spies                                  **
 ** Contact: https://photoqt.org                                         **
 **                                                                      **
 ** This file is part of PhotoQt.                                        **
 **                                                                      **
 ** PhotoQt is free software: you can redistribute it and/or modify      **
 ** it under the terms of the GNU General Public License as published by **
 ** the Free Software Foundation, either version 2 of the License, or    **
 ** (at your option) any later version.                                  **
 **                                                                      **
 ** PhotoQt is distributed in the hope that it will be useful,           **
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of       **
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        **
 ** GNU General Public License for more details.                         **
 **                                                                      **
 ** You should have received a copy of the GNU General Public License    **
 ** along with PhotoQt. If not, see <http://www.gnu.org/licenses/>.      **
 **                                                                      **
 **************************************************************************/

#include <fileplugins/pqc_fileplugin_audio.h>

PQCFilePluginAudio::PQCFilePluginAudio() {

#if defined(PQMQTMULTIMEDIA) || defined(PQMLIBMPV)
    setData({
         {34432,
            {{"Free Lossless Audio Codec"}, {"flac"}, {"audio/flac"}}},
         {33243,
            {{"MP3"}, {"mp3", "mpga", "bit"}, {"audio/mpeg", "audio/MPA", "audio/mpa-robust"}}},
         {11433,
            {{"Xiph.org OGG format"}, {"ogg"}, {"audio/ogg", "application/ogg"}}},
         {11444,
            {{"Waveform Audio File Format"}, {"wav"}, {"audio/x-wav", "audio/vnd.wave", "audio/wav", "audio/wave"}}},
         {11441,
            {{"Advanced Audio Coding"}, {"aac", "m4a", "m4b"}, {"audio/x-hx-aac-adts"}}},
         {66622,
            {{"Windows Media Audio"}, {"wma"}, {"audio/x-ms-wma"}}}});
#endif

}
