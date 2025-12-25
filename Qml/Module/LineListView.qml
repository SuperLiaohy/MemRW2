import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    padding: 8

    property int count: 0

    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        RowLayout {
            Layout.fillWidth: true
            FluText {
                text: qsTr("曲线列表")
                font.bold: true
                font.pixelSize: 14
            }
            Item {
                Layout.fillWidth: true
            }
            FluText {
                text: count + qsTr(" 条")
                font.pixelSize: 11
                color: FluTheme.dark ? "#888" : "#666"
            }
        }
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: FluTheme.dark ? "#444" : "#ddd"
        }
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            id: lineList
            clip: true
            model: lineModel
            delegate: Rectangle {
                id: delegateRoot
                width: lineList.width
                height: 70
                color: {
                    if (mouseArea.pressed) {
                        return FluTheme.dark ? "#3a3a3a" : "#e0e0e0"
                    } else if (mouseArea.containsMouse) {
                        return FluTheme.dark ? "#323232" : "#ebebeb"
                    } else {
                        return FluTheme.dark ? "#2a2a2a" : "#f5f5f5"
                    }
                }
                radius: 4
                border.color: FluTheme.dark ? "#444" : "#ddd"
                border.width: 1

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor

                    onClicked: {
                        dialogLoader.active = true;
                        dialogLoader.item.open(model)
                    }
                }
                RowLayout {
                    anchors. fill: parent
                    anchors.margins: 8
                    spacing: 8
                    Rectangle {
                        width: 4
                        Layout.fillHeight: true
                        color: model.color
                        radius: 2
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 4
                        FluText {
                            text: model.name
                            font. pixelSize: 13
                            font.bold: true
                        }
                        FluText {
                            text: qsTr("数据:  ") + model.cap + qsTr(" 点")
                            font. pixelSize: 10
                            color: FluTheme.dark ? "#888" : "#666"
                        }
                        FluText {
                            text: qsTr("绘制:  ") + model.len + qsTr(" 点")
                            font.pixelSize: 10
                            font.bold: true
                            color: {
                                if (model.len === 0) return FluColors.Red.normal
                                if (model.len > 5000) return FluColors.Orange.normal
                                return FluColors.Green.normal
                            }
                        }
                    }
                    FluCheckBox {
                        checked: model.visible
                        onCheckedChanged: {
                            // ChartLineManager.setLineVisible(modelData, checked)
                            // updatePath()
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: function (event) {
                                event.accepted = true
                                parent.checked = !parent.checked
                            }
                        }
                    }
                    FluIconButton {
                        iconSource: FluentIcons.Delete
                        iconSize: 14
                        MouseArea {
                            anchors.fill: parent
                            onClicked: function (event) {
                                event.accepted = true
                                lineModel.remove(index);
                                // ChartLineManager.removeLine(modelData)
                            }
                        }
                    }
                }
                FluIcon {
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 5
                    iconSource: FluentIcons.Edit
                    iconSize: 12
                    opacity: mouseArea.containsMouse ? 0.6 : 0
                    Behavior on opacity {
                        NumberAnimation {
                            duration: 150
                        }
                    }
                }
            }
        }
    }

    Loader {
        id: dialogLoader
        active: false
        sourceComponent: LineDialog {
            running: false
            visible: false
        }
    }
}
