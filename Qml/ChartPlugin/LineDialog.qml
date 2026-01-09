//
// LinePropertyDialog.qml
// 曲线属性编辑对话框
//

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import FluentUI

FluWindow {
    id: dialog

    width: 500
    height: 650
    title: qsTr("曲线属性编辑")

    minimumWidth: 400
    minimumHeight: 500

    enum OpenMode {
        Change,
        Add
    }

    property int mode: LineDialog.OpenMode.Change
    property var lineAttrModel: null
    property bool running: false
    // ⭐ 临时编辑属性
    property var modelObject: null
    property string editName: ""
    property color editColor: "#409EFF"
    property int editBufferSize: 5000
    // temp show
    property string variName: ""
    property int variAddr: 0
    property string variType: ""
    property int variSize: 0

    // ⭐ 预设颜色
    property var presetColors: [
        "#409EFF", "#67C23A", "#E6A23C", "#F56C6C",
        "#909399", "#C71585", "#FF4500", "#1E90FF",
        "#32CD32", "#FF69B4", "#00CED1", "#FFD700"
    ]
    function open(model,object, openMode) {
        // 加载当前属性
        if (openMode===LineDialog.OpenMode.Add) {
            editName = object.display
            editColor = "red"
            editBufferSize = 5000
            console.log("Opening dialog for vari:", "name:", editName, "OpenMode: Add")
            mode = openMode
            visible = true
            lineAttrModel = model;

            variName = object.display
            variType = object.type
            variSize = object.size
            variAddr = object.addr

        } else if (openMode===LineDialog.OpenMode.Change) {
            editName = object.name
            editColor = object.color
            editBufferSize = object.capacity
            modelObject = object
            console.log("Opening dialog for line:", "name:", editName, "color:", editColor, "OpenMode: Change")
            mode = openMode
            visible = true
            lineAttrModel = model;

            variName = object.variName
            variType = object.variType
            variAddr = object.variAddr
            variSize = object.variSize
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        // ⭐ 内容区域
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            ColumnLayout {
                width: parent.width
                spacing: 20

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    FluText {
                        text: qsTr("变量名称: ") + dialog.variName
                        font.pixelSize: 14
                        font.bold: true
                    }
                    FluText {
                        text: qsTr("变量类型: ") + dialog.variType
                        font.pixelSize: 14
                        font.bold: true
                    }
                    FluText {
                        text: qsTr("变量地址: ") + dialog.variAddr
                        font.pixelSize: 14
                        font.bold: true
                    }
                    FluText {
                        text: qsTr("变量打小: ") + dialog.variSize
                        font.pixelSize: 14
                        font.bold: true
                    }
                }

                // 曲线名称
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    FluText {
                        text: qsTr("曲线名称")
                        font.pixelSize: 14
                        font.bold: true
                    }

                    FluTextBox {
                        id: nameInput
                        Layout.fillWidth: true
                        placeholderText: qsTr("请输入曲线名称")
                        text: dialog.editName

                        onTextChanged: {
                            dialog.editName = text
                        }
                    }
                }
                // 曲线颜色
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    FluText {
                        text: qsTr("曲线颜色")
                        font.pixelSize: 14
                        font.bold: true
                    }
                    RowLayout {
                        spacing: 10
                        // 当前颜色预览
                        Rectangle {
                            width: 80
                            height: 40
                            color: dialog.editColor
                            border.color: FluTheme.dark ? "#666" : "#ccc"
                            border.width: 2
                            radius: 6
                            FluText {
                                anchors.centerIn: parent
                                text: dialog.editColor.toString().toUpperCase()
                                font.pixelSize: 10
                                font.bold: true
                                color: {
                                    var c = dialog.editColor
                                    var r = c.r * 255
                                    var g = c.g * 255
                                    var b = c.b * 255
                                    var brightness = (r * 299 + g * 587 + b * 114) / 1000
                                    return brightness > 128 ? "#000" : "#fff"
                                }
                            }
                        }
                        FluFilledButton {
                            text:  qsTr("自定义颜色")
                            onClicked: colorDialog.open()
                        }
                    }
                    FluText {
                        text: qsTr("预设颜色")
                        font.pixelSize: 12
                        color: FluTheme.dark ? "#888" : "#666"
                    }
                    // 预设颜色网格
                    Grid {
                        columns: 6
                        rowSpacing: 8
                        columnSpacing: 8
                        Repeater {
                            model:  dialog.presetColors
                            Rectangle {
                                width: 40
                                height: 40
                                color: modelData
                                radius: 6
                                border.width: dialog.editColor.toString() === modelData ? 3 : 1
                                border.color: dialog.editColor.toString() === modelData ?
                                    "#fff" :
                                    (FluTheme.dark ? "#555" : "#ccc")
                                // 选中标记
                                FluIcon {
                                    visible: dialog.editColor.toString() === modelData
                                    anchors.centerIn: parent
                                    iconSource: FluentIcons.CheckMark
                                    iconSize: 20
                                    color: {
                                        var c = Qt.color(modelData)
                                        var r = c.r * 255
                                        var g = c.g * 255
                                        var b = c.b * 255
                                        var brightness = (r * 299 + g * 587 + b * 114) / 1000
                                        return brightness > 128 ? "#000" : "#fff"
                                    }
                                }
                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: {
                                        dialog.editColor = modelData
                                    }
                                }
                            }
                        }
                    }
                }
                // 缓冲区大小
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    FluText {
                        text: qsTr("缓冲区大小")
                        font.pixelSize: 14
                        font.bold: true
                    }
                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        FluSlider {
                            Layout.fillWidth: true
                            from: 1000
                            to: 20000
                            stepSize: 1000
                            value: dialog.editBufferSize
                            enabled: !dialog.running
                            opacity: enabled ? 1.0 : 0.5
                            onMoved: {
                                dialog.editBufferSize = value
                            }
                        }
                        FluText {
                            text: dialog.editBufferSize + qsTr(" 点")
                            font.pixelSize: 14
                            font.bold: true
                            Layout.preferredWidth: 80
                            color: dialog.running ? FluColors.Orange.normal : FluTheme.fontPrimaryColor
                        }
                    }

                    // ⭐ 警告提示
                    FluText {
                        text: dialog.running ?
                            qsTr("⚠️ 修改缓冲区大小会清空历史数据"):
                            qsTr("⚠️ 请暂停数据采集后再修改缓冲区大小")
                        font.pixelSize: 11
                        color: FluColors.Orange.normal
                        visible: true
                    }
                }
                // 分隔线
                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: FluTheme.dark ? "#444" : "#ddd"
                }
                // 预览
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    FluText {
                        text: qsTr("预览效果")
                        font.pixelSize: 14
                        font.bold: true
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        height: 80
                        color: FluTheme.dark ? "#1a1a1a" : "#fafafa"
                        border.color: FluTheme.dark ?  "#333" : "#ddd"
                        border.width: 1
                        radius: 6
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 15
                            spacing: 12
                            Rectangle {
                                width: 6
                                Layout.fillHeight: true
                                color: dialog.editColor
                                radius: 3
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 4
                                FluText {
                                    text: dialog.editName || qsTr("未命名曲线")
                                    font.pixelSize: 16
                                    font.bold: true
                                }
                                FluText {
                                    text:  qsTr("缓冲区: ") + dialog.editBufferSize + qsTr(" 点 | 颜色: ") + dialog.editColor.toString().toUpperCase()
                                    font.pixelSize: 12
                                    color:  FluTheme.dark ? "#888" : "#666"
                                }
                            }
                        }
                    }
                }
            }
        }

        // ⭐ 底部按钮区域
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: FluTheme.dark ? "#444" : "#ddd"
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            Item { Layout.fillWidth: true }

            FluButton {
                text:  qsTr("取消")
                Layout.preferredWidth: 100
                onClicked: {
                    console.log("Canceled")
                    dialog.visible = false;
                }
            }

            FluFilledButton {
                text: qsTr("确定")
                Layout.preferredWidth: 100
                onClicked: {
                    console.log("=== Saving properties ===")
                    console.log("  name:", dialog.editName)
                    console.log("  color:", dialog.editColor)
                    console.log("  buffer:", dialog.editBufferSize)
                    if (dialog.mode===LineDialog.OpenMode.Change) {
                        modelObject.name = dialog.editName
                        modelObject.color = dialog.editColor
                        modelObject.capacity = dialog.editBufferSize
                    } else if (dialog.mode===LineDialog.OpenMode.Add) {
                        dialog.lineAttrModel.appendLine(dialog.editName, dialog.editColor, dialog.editBufferSize, variName,variType,variAddr,variSize);
                        showSuccess("Add Succeed "+dialog.editName)
                    }
                    console.log("=== Properties saved ===")
                    dialog.visible = false;
                }
            }
        }
    }

    // 颜色选择对话框
    ColorDialog {
        id:  colorDialog
        title: "选择曲线颜色"

        onAccepted: {
            dialog.editColor = selectedColor
        }
    }
}