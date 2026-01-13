import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import FluentUI

FluFrame {
    id: frame
    padding: 10
    clip: true

    // expose to control
    property bool showLineList: true
    property alias showCrosshair: tipsLineBox.checked
    property int targetFps: 30
    property alias flow: dragCheckedBox.checked
    property alias log: logCheckedBox.checked
    property alias logFile: logFileText.text
    required property var onResetClicked

    // expose to show
    property int lineCount: 0
    required property int realFps
    required property bool running
    required property real viewXMin
    required property real viewXMax
    required property real viewYMin
    required property real viewYMax
    required property bool mouseInChart
    required property real mouseXValue
    required property real mouseYValue


    RowLayout {
        anchors.fill: parent
        spacing: 10
        FluButton {
            text: frame.showLineList ? qsTr("隐藏列表") : qsTr("显示列表")
            Layout.preferredWidth: 75
            onClicked:  {
                frame.showLineList = !frame.showLineList
            }
        }
        FluButton {
            id: resetBtn
            text: qsTr("恢复初始")
            Layout.preferredWidth: 75
            onClicked: frame.onResetClicked()
        }
        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ? "#444" : "#ddd" }
        FluText { text: "Target FPS:" }
        FluSlider {
            from: 15; to: 120
            value: 30
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
        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ? "#444" : "#ddd" }
        FluCheckBox{
            id: dragCheckedBox
            checked: true
            text: "flow"
            textRight: false
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
        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ?  "#444" : "#ddd" }
        FluCheckBox {
            id: tipsLineBox
            checked: false
            text: "tips Line: "
            textRight: false
        }

        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ? "#444" : "#ddd" }
        FluCheckBox{
            enabled: !frame.running
            id: logCheckedBox
            checked: false
            text: "Log File: "
            textRight: false
        }
        FluIconButton {
            enabled: !frame.running
            id: logFileSelector
            iconSource: FluentIcons.FileExplorer
            iconSize: 14
            onClicked: {
                fileDialog.open()
            }

            FluText {
                anchors.bottom: logFileSelector.top; anchors.bottomMargin: -5; anchors.horizontalCenter:logFileSelector.horizontalCenter;
                visible: logFileSelector.hovered;  id: logFileText;font.pixelSize: 14;
            }
        }


        Item { Layout.fillWidth: true }
        FluText {
            text: qsTr("视图: ") + (frame.viewXMax-frame.viewXMin).toFixed(3) + "ms"
            font.bold: true
        }
        FluText {
            text: "Y: " + (frame.viewYMax - frame.viewYMin).toFixed(3)
            color: FluColors.Blue.normal
        }
        Rectangle { width: 1; height: 25; color: FluTheme.dark ? "#444" : "#ddd" }
        // ⭐ 鼠标位置显示相对时间
        FluText {
            text: "T: " + frame.mouseXValue.toFixed(3)
            font.pixelSize: 11
            color: FluTheme.dark ? "#888" : "#666"
        }
        FluText {
            text: "V: " + frame.mouseYValue.toFixed(3)
            font.pixelSize: 11
            color: FluColors.Blue.normal
        }
        Rectangle { width: 1; height: 25; color: FluTheme.dark ? "#444" : "#ddd" }
        FluText {
            id: fpsText
            text: "FPS: " + frame.realFps
            color: FluColors.Green.normal
            font.bold: true
        }
    }

    FileDialog {
        id: fileDialog
        nameFilters: ["csv files [*.csv] (*.csv)","txt files [*.txt] (*.txt)", "all files [*.*] (*.*)"]

        onAccepted: {
            console.log("selected:", selectedFile)
            logFileText.text = selectedFile;
        }
    }

}
