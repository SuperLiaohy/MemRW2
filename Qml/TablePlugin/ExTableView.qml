import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI


FluFrame {
    id: frame
    padding: 8

    ExTableModel {
        id: tableVariModel
    }

    property var variColumnWidths: [0.3, 0.25, 0.3, 0.15]
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
            FluText { text: "Target FPS:" }
            FluSlider {
                from: 1; to: 80
                value: 30
                stepSize: 1
                Layout.preferredWidth: 50
                onValueChanged: {
                    tableVariModel.targetFps = value
                }
            }
            FluText {
                text: tableVariModel.targetFps
                Layout.preferredWidth: 18
            }

            Item {
                Layout.fillWidth: true
            }
            FluIconButton {
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
        HorizontalHeaderView {
            id: variHeader
            syncView: tableVariView
            Layout.fillWidth: true
            Layout.preferredHeight: 50

            clip: true
            boundsMovement:  Flickable.StopAtBounds
            boundsBehavior:  Flickable.StopAtBounds

            property real dragLeftWidth: 0
            property real dragRightWidth: 0
            property real minColumn: 0.08
            property real dragX: 0

            delegate: Item {
                clip: true
                implicitWidth:  variColumnWidths[column] * variHeader.width
                implicitHeight: headerLabel.implicitHeight * 2

                FluRectangle {
                    id: headerBackground
                    anchors. fill: parent
                    color: FluTheme.dark ? "#2a2a2a" : "#f0f0f0"
                    borderColor: FluTheme.dark ? "#444742" : "#99cadf"
                    borderWidth: 1
                    radius: rad(column)

                    function rad(col) {
                        if (col === 0) return [10, 0, 0, 10]
                        if (col === 3) return [0, 10, 10, 0]
                        return [0, 0, 0, 0]
                    }
                }
                FluText {
                    id: headerLabel
                    anchors.centerIn: parent
                    clip: true
                    font.pixelSize: 20
                    text: model.display
                }
                MouseArea {
                    id: headerDragHandler
                    enabled: tableVariView.rows > 0
                    visible: column < 4
                    width: 10
                    anchors.right: parent.right
                    anchors.rightMargin: -width / 2
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    cursorShape: Qt.SplitHCursor
                    z: 100  // 确保在最上层可点击

                    onPressed: {
                        variHeader.dragLeftWidth = variColumnWidths[column]
                        variHeader.dragRightWidth = variColumnWidths[column + 1]
                        variHeader.dragX = mapToGlobal(mouseX, mouseY).x
                    }
                    onPositionChanged: {
                        if (pressed) {
                            let delta = (variHeader.dragX - mapToGlobal(mouseX, mouseY).x) / variHeader.width
                            let newLeft = Math. max(variHeader. minColumn, variHeader.dragLeftWidth - delta)
                            let newRight = Math.max(variHeader.minColumn, variHeader.dragRightWidth + variHeader. dragLeftWidth - newLeft)
                            newLeft = variHeader.dragRightWidth + variHeader.dragLeftWidth - newRight

                            // 关键：创建新数组触发绑定更新
                            var newWidths = variColumnWidths.slice()
                            newWidths[column] = newLeft
                            newWidths[column + 1] = newRight
                            variColumnWidths = newWidths

                            tableVariView.forceLayout()
                            variHeader.forceLayout()
                        }
                    }
                }
            }
        }

        TableView {
            id: tableVariView
            Layout.fillWidth: true
            Layout.fillHeight: true

            columnWidthProvider: function(column) {
                return variColumnWidths[column] * width
            }

            // 关键：宽度变化时强制重新布局
            onWidthChanged: forceLayout()

            model: tableVariModel
            clip: true
            boundsMovement: Flickable.StopAtBounds
            boundsBehavior: Flickable.StopAtBounds

            selectionModel: ItemSelectionModel {
                id: variSelModel
                model: tableVariView.model
            }

            delegate: Item {
                id: variControl

                implicitWidth: variColumnWidths[column] * tableVariView.width
                implicitHeight:  42

                readonly property real padding: 5

                required property int row
                required property int column
                required property bool current
                required property var model

                FluRectangle {
                    id: variBackground
                    anchors.fill: parent
                    color: {
                        if (variControl.current)
                            return palette.highlight
                        return FluTheme.dark ? "#1e1e1e" :  "#e0e0e0"
                    }
                    opacity:  variControl.current ?  0.6 : 0.3
                    borderColor:  "#145eef"
                    borderWidth:  variControl.current ?  1 : 0
                    radius:  rad(variControl. column)

                    function rad(col) {
                        if (col === 0) return [7, 0, 0, 7]
                        if (col === 3) return [0, 7, 7, 0]
                        return 0
                    }
                }
                property real value;
                Loader {
                    anchors.fill: parent
                    anchors. margins: variControl.padding

                    sourceComponent: {
                        switch (variControl.column) {
                            case 0: return nameComponent
                            case 1: return valueComponent
                            case 2: return editableValueComponent
                            case 3: return deleteBtnComponent
                        }
                    }
                }

                Component {
                    id: nameComponent
                    FluText {
                        text: variControl.model.display
                        anchors.centerIn: parent
                        clip: true
                        font.pixelSize: 17
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }

                Component {
                    id: valueComponent
                    FluText {
                        text:  Number(variControl.model.display).toFixed(5)
                        anchors. centerIn: parent
                        clip: true

                        font.pixelSize: 17
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
                // 可编辑数值组件
                Component {
                    id: editableValueComponent
                    RowLayout {
                        anchors.fill: parent
                        TextField {
                            Layout.fillWidth: true
                            id: editField
                            anchors.margins: 2
                            text: Number(variControl.model.display)
                            validator: DoubleValidator {
                                notation: DoubleValidator.StandardNotation
                            }
                            inputMethodHints: Qt.ImhFormattedNumbersOnly
                            Keys.onReturnPressed: {
                                focus = false
                            }
                            Keys.onEnterPressed: {
                                focus = false
                            }
                        }
                        FluIconButton {
                            iconSource: FluentIcons.Accept
                            iconSize: 16
                            onClicked: {
                                if (!Backend.running) {
                                    showError("not running")
                                    return;
                                }
                                let value = parseFloat(editField.text)
                                if (!isNaN(value)) {
                                    variControl.model.edit = value
                                    console.log(`行${variControl.row + 1} - write`)
                                    if (tableVariModel.sendWriteRequest(row)) {
                                        showSuccess("write memory successfully")
                                    } else {
                                        showError("Cannot support variable address misalignment, length greater than 4, or communication errors")
                                    }
                                } else {
                                    editField.text = Number(variControl.model.display)
                                }
                            }
                        }
                    }
                }
                Component {
                    id: deleteBtnComponent
                    FluIconButton {
                        anchors.centerIn: parent
                        iconSource: FluentIcons.Delete
                        iconSize: 16
                        onClicked: {
                            tableVariModel.removeRow(row)
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
