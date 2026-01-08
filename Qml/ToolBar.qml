import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    RowLayout {
        anchors.fill: parent
        spacing: 10
        FluFilledButton {
            text: Backend.connected ? qsTr("断开") : qsTr("连接")
            Layout.preferredWidth: 65
            onClicked: {
                if (!Backend.connected) {
                    let res = Backend.connect()
                    if (res) showSuccess("connection succeed")
                    else showError("connection failed")
                } else {
                    Backend.disconnect()
                    showSuccess("disconnection")
                }
            }
        }
        FluFilledButton {
            text: Backend.running && Backend.connected ? qsTr("暂停") : qsTr("开始")
            enabled: Backend.connected
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
        Item { Layout.fillWidth: true }
        FluText {
            text: Backend.running ? qsTr("● 采集中") : qsTr("○ 已暂停")
            color: Backend.running ? FluColors.Green.normal : FluColors.Orange.normal
            font.bold: true
        }
    }
}
