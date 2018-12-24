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

        Button {
              id: playPauseButton
              property bool playing: true
              anchors {
                    left: parent.left
                    bottom: parent.bottom
                    margins: 10
              }

              iconName: "Play"
              iconSource: playing?"qrc:///images/pause-64.png":"qrc:///images/play-64.png"

              width: Math.max(parent.width, parent.height) / 25
              height: Math.min(parent.width, parent.height) / 25
              opacity: 1.0

              z: 2.0

              onClicked: {
                  console.log("playing " + playing)

                  if (playing) {
                        player.pause()
                        playPauseButton.iconSource = "qrc:///images/play-64.png"
                  } else {
                        player.play()
                        playPauseButton.iconSource = "qrc:///images/pause-64.png"
                  }

                  console.log("img src " + iconSource)
                  playing = !playing
                }
        }

        SeekControl {
            anchors {
                left: playPauseButton.right
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
