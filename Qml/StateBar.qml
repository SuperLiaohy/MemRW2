import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    padding: 10

    property bool running: false
    property int lineCount: 0
    property real viewTimeAxisRange: 0

    property real viewValueAxisMin: 0
    property real viewValueAxisMax: 0

    property bool mouseInChart: false
    property point mousePoint: Qt.point(0,0)

    property int fps: 0

    RowLayout {
        anchors.fill: parent
        spacing: 12
        FluText {
            text: frame.running ? qsTr("● 采集中") : qsTr("○ 已暂停")
            color: frame.running ? FluColors.Green.normal : FluColors.Orange.normal
            font.bold: true
        }
        Rectangle { width: 1; height: 25; color: FluTheme.dark ? "#444" : "#ddd" }
        FluText {
            text: qsTr("曲线: ") + frame.lineCount
            font.bold: true
        }
        Rectangle { width: 1; height:  25; color: FluTheme.dark ? "#444" :  "#ddd" }
        FluText {
            text: qsTr("视图: ") + frame.viewTimeAxisRange + "ms"
            font.bold: true
        }
        FluText {
            text: "Y: " + frame.viewValueAxisMin.toFixed(0) + "~" + frame.viewValueAxisMax.toFixed(0)
            color: (frame.viewValueAxisMax - frame.viewValueAxisMin) < 99 ? FluColors.Blue.normal : FluTheme.fontPrimaryColor
        }
        Rectangle { width: 1; height: 25; color: FluTheme.dark ? "#444" : "#ddd" }
        // ⭐ 鼠标位置显示相对时间
        FluText {
            visible: frame.mouseInChart
            text: "T: " + frame.mousePoint.x
            font.pixelSize: 11
            color: FluTheme.dark ? "#888" : "#666"
        }
        FluText {
            visible: frame.mouseInChart
            text: "V: " + frame.mousePoint.y
            font.pixelSize: 11
            color: FluColors.Blue.normal
        }
        Item { Layout.fillWidth: true }
        FluText {
            id: fpsText
            text: "FPS: " + fps
            color: FluColors.Green.normal
            font.bold: true
        }
    }
}
