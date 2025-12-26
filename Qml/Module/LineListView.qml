import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

// the structure is two layers ListView, the fist View is for showing the groups, and the second View is for showing the lines
// overall layout is ColumnLayout, each List View is paired with RowLayout header and rectangle separator
// need
FluFrame {
    id: frame
    padding: 8
    property int count: 0
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
                iconSource: FluentIcons.Add
                iconSize: 11
                MouseArea {
                    anchors.fill: parent
                    onClicked: function (event) {
                        event.accepted = true
                        dialogGroup.open()
                    }
                }
            }
            FluText {
                text: groupView.count + qsTr(" 组")
                font.pixelSize: 11
                color: FluTheme.dark ? "#888" : "#666"
            }
            FluText {
                text: LineGroupModel.lineCount + qsTr(" 条")
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
            Layout.fillWidth: true
            Layout.fillHeight: true
            id: groupView
            clip: true
            model: LineGroupModel
            delegate: Rectangle {
                id: group
                width: groupView.width
                height: header.height + lineView.height + 9
                color: "transparent"
                radius: 4
                border.color: FluTheme.dark ? "#444" : "#ddd"
                border.width: 1
                // inner layout
                Column {
                    padding: 4
                    anchors.fill: parent
                    spacing: 4
                    // inner header
                    RowLayout {
                        id: header
                        width: parent.width
                        FluIcon {
                            iconSource: lineView.visible ? FluentIcons.ChevronDown : FluentIcons.ChevronRight
                            iconSize: 12
                            MouseArea {
                                anchors.fill: parent
                                onClicked: function (event) {
                                    event.accepted = true
                                    lineView.visible = !lineView.visible
                                }
                            }
                        }
                        FluText {
                            id: title
                            text: qsTr(model.name)
                            font.bold: true
                            font.pixelSize: 12
                        }
                        Item {
                            Layout.fillWidth: true
                        }
                        FluIconButton {
                            iconSource: FluentIcons.Add
                            iconSize: 9
                            MouseArea {
                                anchors.fill: parent
                                onClicked: function (event) {
                                    event.accepted = true
                                    sheet.openWithModel(lineModel)
                                }
                            }
                        }
                        FluIconButton {
                            iconSource: FluentIcons.Delete
                            iconSize: 9
                            MouseArea {
                                anchors.fill: parent
                                onClicked: function (event) {
                                    event.accepted = true
                                    LineGroupModel.removeGroup(index)
                                }
                            }
                        }
                        FluText {
                            Layout.rightMargin: 8
                            text: lineView.count + qsTr(" 条")
                            font.pixelSize: 9
                            color: FluTheme.dark ? "#888" : "#666"
                        }
                    }
                    // inner rectangle separator
                    Rectangle {
                        width: parent.width
                        anchors.horizontalCenter: parent.horizontalCenter
                        height: 1
                        color: FluTheme.dark ? "#444" : "#ddd"
                        visible: lineView.visible
                    }
                    // inner ListView
                    ListView {
                        model: lineModel
                        width: parent.width
                        height: visible ? contentHeight + 10 : 0
                        id: lineView
                        clip: false
                        visible: false
                        interactive: false
                        delegate: Rectangle {
                            id: delegateRoot
                            width: parent ? parent.width - 10 : 0
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
                                    dialogLoader.active = true;
                                    dialogLoader.item.open(model)
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
                                        model.visible = checked
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
                                            lineModel.removeLine(index);
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
            }
        }
    }

    FluContentDialog {
        id: dialogGroup
        title: qsTr("group name")
        property string tempName
        onOpened: {
            tempName = ""
        }
        focus: true
        contentDelegate: Component {
            FluTextBox {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                focus: true
                placeholderText: qsTr("please input group name")
                onTextChanged: {
                    dialogGroup.tempName = text;
                }
                Keys.onReturnPressed: {
                    dialogGroup.positiveClicked()
                    dialogGroup.close()
                }
            }
        }
        onNegativeClicked: {}
        onPositiveClicked: {
            if (tempName === "") {
                showError(qsTr("name can not be empty"))
                return;
            }
            if (LineGroupModel.appendGroup(tempName)) {
                showSuccess(qsTr("append group " + tempName))
            } else {
                showError(qsTr(tempName + "already exists"))
            }
        }

    }
}
