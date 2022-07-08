import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls
import AAAAA
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    AudioProcess {
        id: audioProcess
    }

    Button {
        x: 100
        y: 100
        width: 100
        height: 60
        text: "开始"
        onClicked: {
            audioProcess.play();
        }
    }

    Button {
        x: 300
        y: 100
        width: 100
        height: 60
        text: "停止"

        onClicked: {
            audioProcess.play2();
        }
    }
}
