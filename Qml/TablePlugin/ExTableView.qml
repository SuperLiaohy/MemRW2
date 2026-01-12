import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI


FluFrame {
    id: frame
    padding: 8

    property bool running: false

    ExTableModel {
        id: tableVariModel
    }

    // overall layout
    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        // outside header
        RowLayout {
            Layout.fillWidth: true
            FluText {
                text: qsTr("Table")
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
                text: tableVariView.rows + qsTr(" 个")
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
        TableView {
            id: tableVariView
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: tableVariModel

            delegate: Rectangle {
                implicitWidth: 130
                implicitHeight: 42
                border.color: "#ccc"
                Loader {
                    sourceComponent: {
                        switch (column) {
                            case 0:
                                return nameComponent
                            case 1:
                                return valueComponent
                            case 2:
                                return valueComponent
                            case 3:
                                return okBtnComponent
                            case 4:
                                return deleteBtnComponent
                        }
                    }

                    Component {
                        id: nameComponent
                        FluText {
                            text: display; anchors.centerIn: parent; anchors.fill: parent
                        }
                    }
                    Component {
                        id: valueComponent
                        FluText {
                            text: Number(display).toFixed(5)
                            anchors.centerIn: parent; anchors.fill: parent
                        }
                    }
                    Component {
                        id: okBtnComponent
                        Button {
                            anchors.centerIn: parent; anchors.fill: parent
                            text: "write"
                            onClicked: console.log(`行${row + 1} - ${text}`)
                        }
                    }
                    Component {
                        id: deleteBtnComponent
                        Button {
                            anchors.centerIn: parent; anchors.fill: parent
                            text: "delete"
                            onClicked: console.log(`行${row + 1} - ${text}`)
                        }
                    }
                }
            }
        }
    }
    function appendAction(modelNode) {
        tableVariDialog.openAddNode(modelNode)
    }


    FluContentDialog {
        id: tableVariDialog
        title: qsTr("添加Table变量")
        property var node
        property string editName: ""
        width: 250
        contentDelegate: Component {
            Item {
                anchors.fill: parent
                implicitHeight: 150
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 2
                    FluText {
                        text: qsTr("变量名称: ") + tableVariDialog.node.name
                        font.pixelSize: 12
                    }
                    FluText {
                        text: qsTr("变量类型: ") + tableVariDialog.node.type
                        font.pixelSize: 12
                    }
                    FluText {
                        text: qsTr("变量地址: 0x") + tableVariDialog.node.addr.toString(16)
                        font.pixelSize: 12
                    }
                    FluText {
                        text: qsTr("变量大小: ") + tableVariDialog.node.size
                        font.pixelSize: 12
                    }
                    RowLayout {
                        FluText {text: "name: "}
                        TextField {
                            Layout.fillWidth: true
                            placeholderText: "Input name"
                            text: tableVariDialog.node.name
                            onTextChanged: {
                                tableVariDialog.editName = text
                            }
                        }
                    }

                }
            }
        }

        onPositiveClicked: {
            tableVariModel.appendRow(editName, node.name, node.type, node.addr, node.size);
            showSuccess("Successfully added " + editName)
        }
        function openAddNode(n) {
            tableVariDialog.node = n
            tableVariDialog.open()
        }
    }
}
