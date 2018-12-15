/****************************************************************************
* VLC-Qt - Qt and libvlc connector library
* Copyright (C) 2016 Tadej Novak <tadej@tano.si>
*
* This library is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

import QtQuick 2.3
import QtQuick.Controls 1.0
import VLCQt 1.1

ApplicationWindow {
    id: tp
    title: "qDonkey"
    width:  1024
    height: 768
    visible: true

    Rectangle {
        width: 1024
        height: 768
        color: "black"

        Component.onCompleted: {
            console.warn("started")
        }

        VlcPlayer {
            id: player
            logLevel: Vlc.DisabledLevel
            url: "file:/home/inkpot/Downloads/BigBuckBunny_512kb.mp4"
            onUrlChanged: {
                console.warn("file opened")
            }

            Component.onCompleted: {
                console.log("VlcPlayer completed " + url)
                console.log("length " + length);
            }
        }

        VlcVideoOutput {
            id: video
            source: player
            anchors.fill: parent
        }

        SeekControl {
            anchors {
                left: parent.left
                right: parent.right
                margins: 10
                bottom: parent.bottom
            }

            duration: (player.length !== -1)?player.length:0
            playPosition: player.length !== -1 ? player.position*player.length : 0
            onSeekPositionChanged: player.position = player.length!==0?(1.0*seekPosition)/player.length:0.0
        }
    }
}
