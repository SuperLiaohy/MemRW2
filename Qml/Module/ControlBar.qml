import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    property bool running: false
    property bool showLineList: false
    property bool showCrosshair: false
    property int targetFps: 0
    RowLayout {
        anchors.fill: parent
        spacing: 10
        FluFilledButton {
            text: Backend.running ? qsTr("暂停") : qsTr("开始")
            Layout.preferredWidth: 65
            onClicked: {
                if (Backend.running) {
                    console.log("=== Paused ===")
                } else {
                    console.log("=== Starting ===")
                }
                Backend.running=!Backend.running
            }
        }
        FluButton {
            text: qsTr("重置")
            Layout.preferredWidth: 55
            onClicked: {

            }
        }
        FluButton {
            text: qsTr("适应")
            Layout.preferredWidth: 50
            // enabled: canInteract
            onClicked: {

            }
        }
        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ? "#444" :  "#ddd" }
        FluText { text: qsTr("曲线:") }
        FluButton {
            text: frame.showLineList ? qsTr("隐藏列表") : qsTr("显示列表")
            Layout.preferredWidth: 75
            // enabled:  > 0
            onClicked:  {
                frame.showLineList = !frame.showLineList
            }
        }
        Rectangle { width: 1; Layout. fillHeight: true; color: FluTheme.dark ? "#444" : "#ddd" }
        FluText { text: "FPS:" }
        FluSlider {
            from: 1; to: 60
            value: frame.targetFps
            stepSize: 1
            Layout.preferredWidth: 50
            onValueChanged: {
                frame.targetFps = value
                // updateTimer.interval = Math.floor(1000 / frame.targetFps)
            }
        }
        FluText {
            text: frame.targetFps
            Layout.preferredWidth: 18
        }
        // FluText { text: "最大绘制:" }
        // FluSlider {
        //     from: 500; to: 10000
        //     value: maxDrawPoints
        //     stepSize: 500
        //     Layout.preferredWidth: 50
        //     onValueChanged: {
        //         maxDrawPoints = value
        //         updatePath()
        //     }
        // }
        // FluText {
        //     text: maxDrawPoints
        //     Layout.preferredWidth: 40
        // }
        // 在控制栏中添加
        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ?  "#444" : "#ddd" }
        FluButton {
            text: frame.showCrosshair ? qsTr("✕ 准线") : qsTr("○ 准线")
            Layout.preferredWidth: 60
            onClicked: {
                frame.showCrosshair = !frame.showCrosshair
            }
        }
        Item { Layout.fillWidth: true }
    }
}
