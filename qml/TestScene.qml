import QtQuick 2.3
import QtQuick.Controls 1.0

ApplicationWindow {
    id: tp
    title: "testScene"
    width:  1024
    height: 768
    visible: true

    Rectangle {
        width: 1024
        height: 768
        color: "black"

        Button {
              id: closeButton
              anchors {
                    left: parent.left
                    bottom: parent.bottom
                    margins: 10
              }
              width: Math.max(parent.width, parent.height) / 25
              height: Math.min(parent.width, parent.height) / 25

              z: 2.0
              text: "Back"
              onClicked: root.close()
        }

        SeekControl {
            anchors {
                left: closeButton.right
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
