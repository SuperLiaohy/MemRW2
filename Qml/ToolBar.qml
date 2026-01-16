import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame

    Connections {
        target: Backend
        function onLinkErrorHappen() {
            connectBtn.clicked();
            showError("A large number of communication errors occurred, resulting in automatic disconnection")
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 10
        FluFilledButton {
            id: connectBtn
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
                    if (autoReloadCheckedBox.checked) {
                        console.log("=== Reload ===")
                        if (!sheet.reloadFile()) return;
                    }
                    console.log("=== Paused ===")
                } else {
                    console.log("=== Starting ===")
                }
                Backend.running=!Backend.running
            }
        }
        FluCheckBox{
            id: autoReloadCheckedBox
            checked: false
            text: "reload before start"
            textRight: false
        }
        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ? "#444" : "#ddd" }
        FluText{text:"delay: "}
        FluSlider {
            Layout.preferredWidth: 100
            enabled: !Backend.running
            opacity: enabled ? 1.0 : 0.5
            text: "us"
            implicitWidth: 100
            from: 0; to: 10000
            value: 0
            stepSize: 50
            onValueChanged: {
                Backend.delayUs = value
            }
        }
        FluText {
            text: Backend.delayUs
            Layout.preferredWidth: 18
        }
        Rectangle { width: 1; Layout.fillHeight: true; color: FluTheme.dark ? "#444" : "#ddd" }
        FluButton {
            text: "reset"
            onClicked: {
                if (Backend.resetTarge()) {
                    showSuccess("reset successfully")
                } else {
                    showError("reset failed")
                }
            }
        }
        Item { Layout.fillWidth: true }
        FluText {
            text: "Hz: " + Backend.samplingHz
            color: FluColors.Blue.normal
            font.bold: true
        }
        Rectangle { width: 1; height: 25; color: FluTheme.dark ? "#444" : "#ddd" }
        FluText {
            text: Backend.running ? qsTr("● 采集中") : qsTr("○ 已暂停")
            color: Backend.running ? FluColors.Green.normal : FluColors.Orange.normal
            font.bold: true
        }
    }
}
