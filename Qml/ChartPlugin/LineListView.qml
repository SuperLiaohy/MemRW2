import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    padding: 8

    property var lineAttrModel

    required property bool running

    // overall layout
    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        // outside header
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
            FluIconButton {
                enabled: !frame.running
                iconSource: FluentIcons.Add
                iconSize: 11
                onClicked: {
                    sheet.openAppendNode(frame.appendAction)
                }
            }
            FluText {
                text: lineAttrView.count + qsTr(" 条")
                font.pixelSize: 11
                color: FluTheme.dark ? "#888" : "#666"
            }
        }
        // outside rectangle separator
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: FluTheme.dark ? "#444" : "#ddd"
        }
        // outside ListView
        ListView {
            id: lineAttrView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: frame.lineAttrModel
            clip: true
            delegate: Rectangle {
                id: delegateRoot
                width: parent ? parent.width : 0
                height: 70
                radius: 4
                border.color: FluTheme.dark ? "#444" : "#ddd"
                border.width: 1
                color: {
                    if (mouseArea.pressed) {
                        return FluTheme.dark ? "#3a3a3a" : "#e0e0e0"
                    } else if (mouseArea.containsMouse) {
                        return FluTheme.dark ? "#323232" : "#ebebeb"
                    } else {
                        return FluTheme.dark ? "#2a2a2a" : "#f5f5f5"
                    }
                }
                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        chartLineDialog.openWithMode(frame.lineAttrModel,model,LineDialog.OpenMode.Change)
                    }
                }
                RowLayout {
                    anchors.fill: parent
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
                            font.pixelSize: 13
                            font.bold: true
                        }
                        FluText {
                            text: qsTr("数据:  ") + model.capacity + qsTr(" 点")
                            font.pixelSize: 10
                            color: FluTheme.dark ? "#888" : "#666"
                        }
                        FluText {
                            text: qsTr("绘制:  ") + model.len + qsTr(" 点")
                            font.pixelSize: 10
                            font.bold: true
                            color: {
                                if (model.paintLen === 0) return FluColors.Red.normal
                                if (model.paintLen > 5000) return FluColors.Orange.normal
                                return FluColors.Green.normal
                            }
                        }
                    }
                    FluCheckBox {
                        checked: true
                        onCheckedChanged: {
                            model.visible = checked
                        }
                    }
                    FluIconButton {
                        iconSource: FluentIcons.Delete
                        iconSize: 14
                        onClicked: {
                            frame.lineAttrModel.removeLine(index);
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
    function appendAction(modelNode) {
        chartLineDialog.openWithMode(frame.lineAttrModel,modelNode,LineDialog.OpenMode.Add)
    }


    LineDialog {
        id: chartLineDialog
        running: frame.running
    }
}
