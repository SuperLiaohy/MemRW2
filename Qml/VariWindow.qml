import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import FluentUI

FluSheet {
    id: sheet

    property var appendAction: function(modelNode) {}

    background: FluRectangle {
        radius: [30,30,0,0]
        FluShadow{
            radius: 0
        }
        borderColor: FluTheme.dark ? Window.active ? Qt.rgba(55/255,55/255,55/255,1):Qt.rgba(45/255,45/255,45/255,1) : Qt.rgba(226/255,229/255,234/255,1)
        color: FluTheme.dark ? Window.active ?  Qt.rgba(38/255,44/255,54/255,1) : Qt.rgba(39/255,39/255,39/255,1) : Qt.rgba(251/255,251/255,253/255,1)
    }

    property int maxSize;

    Rectangle {
        id: dragHandle
        width: parent.width * 0.618/4
        height: 6
        radius: 3
        color: "#80000000"
        opacity: 0.4
        anchors.top: parent.top  // 固定在顶部
        anchors.horizontalCenter: parent.horizontalCenter


        DragHandler {
            cursorShape: Qt.SizeVerCursor
            parent: sheet.header
            xAxis.enabled: false
            property real startHeight:sheet.height
            onActiveChanged: {
                if (active) {
                    startHeight = sheet.height
                } else {
                    sheet.size = sheet.height
                }
            }
            onTranslationChanged: {
                const newHeight = startHeight - translation.y
                sheet.height = Math.max(Math.min(newHeight,maxSize),100)
            }
        }
    }

    RowLayout {
        id: toolBar
        anchors.top: dragHandle.bottom
        anchors.topMargin: 10
        width: parent.width - 30
        anchors.horizontalCenter: parent.horizontalCenter
        FluButton {
            id: loadBtn
            text: qsTr("选择文件")
            onClicked: {
                fileDialog.open()
            }
        }
        FluText {
            Layout.fillWidth: true
            x: loadBtn.width + 20
            id: fileText
            text: "";
        }
        Item { Layout.fillWidth: true }
        FluButton {
            id: reloadBtn
            text: qsTr("重新加载")
            onClicked: {
                sheet.reloadFile()
            }
        }
    }
    RowLayout {
        id: customAdd
        anchors.top: toolBar.bottom
        anchors.left: toolBar.left
        anchors.right: toolBar.right
        height: 45
        FluText {
            text: "Quick variable addition: "
        }
        FluTextBox {
            id: vari
            Layout.fillWidth: true
            placeholderText: "Input variable"
            Keys.onPressed: (event)=> {
                if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    customAdd.search()
                }
            }
        }
        FluIconButton {
            iconSource: FluentIcons.ReturnKey
            onClicked: customAdd.search()
        }
        function search() {
            console.log(vari.text)
            var idx = myTreeModel.findNode(vari.text)
            if (!idx || !idx.valid) {
                showError("Cannot find the variable")
                return
            }
            treeView.expandToIndex(idx)
            Qt.callLater(function() {
                treeView.forceLayout()
                let row = treeView.rowAtIndex(idx)
                if (row >= 0) {
                    treeView.selectionModel.setCurrentIndex(idx, ItemSelectionModel.ClearAndSelect)
                    treeView.positionViewAtRow(row, Qt.AlignVCenter)
                }
            })
            parent.forceActiveFocus()
            showSuccess("Successfully found the variable")
        }
    }

    property var columnWidths: [0.65, 0.15, 0.1, 0.1]
    HorizontalHeaderView {
        id: header
        syncView: treeView
        anchors.top: customAdd.bottom
        anchors.left: customAdd.left
        anchors.right: customAdd.right

        clip: true
        boundsMovement: Flickable.StopAtBounds
        boundsBehavior: Flickable.StopAtBounds

        property real dragLeftWidth: 0
        property real dragRightWidth: 0
        property real minColumn: 0.08
        property real dragX: 0
        onWidthChanged: forceLayout()
        delegate: Item {
            implicitHeight: label.implicitHeight * 2
            FluRectangle {
                id: background
                anchors.fill: parent
                color: FluTheme.dark ? "#2a2a2a" : "#f0f0f0"
                borderColor: FluTheme.dark ?"#444742":"#99cadf"
                borderWidth: 1
                radius: rad(column)

                function rad(column) {
                    if (column === 0) return [10, 0, 0, 10]
                    if (column === 3) return [0, 10, 10, 0]
                    return [0, 0, 0, 0]
                }
            }
            FluText {
                id: label
                anchors.centerIn: parent
                clip: true
                text: model.display
            }
            MouseArea {
                id: dragHandler
                enabled: treeView.model.rowCount()>0
                visible: column < 3
                width: 10
                anchors.right: parent.right
                anchors.rightMargin: -width/2
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                cursorShape: Qt.SplitHCursor
                onPressed: {
                    header.dragLeftWidth = sheet.columnWidths[column]
                    header.dragRightWidth = sheet.columnWidths[column+1]
                    header.dragX = mapToGlobal(mouseX,mouseY).x
                    console.log("drag")
                }
                onPositionChanged: {
                    if (pressed) {
                        let delta = (header.dragX - mapToGlobal(mouseX,mouseY).x)/header.width

                        let newLeft = Math.max(header.minColumn, header.dragLeftWidth - delta)
                        let newRight = Math.max(header.minColumn, header.dragRightWidth + header.dragLeftWidth - newLeft)
                        newLeft = header.dragRightWidth + header.dragLeftWidth - newRight

                        sheet.columnWidths[column] = newLeft
                        sheet.columnWidths[column + 1] = newRight

                        treeView.forceLayout()
                    }
                }
            }
        }
    }

    TreeView {
        id: treeView
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: customAdd.left
        anchors.right: customAdd.right

        columnWidthProvider: function(column) {return columnWidths[column]*width}

        model: myTreeModel
        clip: true
        // 禁止水平方向的任何滚动
        boundsMovement: Flickable.StopAtBounds
        boundsBehavior: Flickable.StopAtBounds

        selectionModel: ItemSelectionModel {
            id: selModel
            model: treeView.model
        }

        delegate: Item {
            id: control
            width:300
            implicitWidth: padding + label.x + label.implicitWidth + padding
            implicitHeight: 30

            readonly property real indentation: 20
            readonly property real padding: 5

            // required by TreeView:
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth
            required property int row
            required property int column
            required property bool current

            // 背景
            FluRectangle {
                id: background
                anchors.fill: parent
                color: {
                    if (control.current)
                        return palette.highlight
                    color: FluTheme.dark ? "#1e1e1e" : "#e0e0e0"
                }
                opacity: control.current ? 0.3 : 0.2
                borderColor: "#145eef"
                borderWidth: control.current ? 1 : 0
                radius: rad(column)

                function rad(col) {
                    if (col === 0)      return [7, 0, 0, 7]
                    if (col === treeView.columnCount - 1) return [0, 7, 7, 0]
                    return 0
                }
            }

            RowLayout {
                anchors.fill: parent
                spacing: 0
                anchors.leftMargin: column===0? padding + depth * indentation:0
                Loader {
                    id: indicatorLoader
                    active: isTreeNode && hasChildren
                    visible: active
                    Layout.alignment: Qt.AlignVCenter
                    sourceComponent: FluIcon {
                        iconSource: expanded ? FluentIcons.ChevronDown : FluentIcons.ChevronRight
                        iconSize: 12
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                let idx = treeView.index(row, column)
                                treeView.selectionModel.setCurrentIndex(idx, ItemSelectionModel.NoUpdate)
                                treeView.toggleExpanded(row)
                            }
                        }
                    }
                }
                FluText {
                    id: label
                    Layout.fillWidth: true
                    Layout.leftMargin: 5
                    Layout.alignment: Qt.AlignVCenter
                    text: model.display
                    clip: true
                    elide: Text.ElideLeft
                }
                Loader {
                    id: addBtnLoader
                    active: !hasChildren && column === 0
                    visible: active
                    Layout.alignment: Qt.AlignVCenter
                    sourceComponent: FluIconButton {
                        iconSource: FluentIcons.Add
                        iconSize: 16
                        onClicked: sheet.appendAction(model)
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        nameFilters: ["dwarf files [*.elf *.axf] (*.elf *.axf)", "all files [*.*] (*.*)"]

        onAccepted: {
            console.log("selected:", selectedFile)
            fileText.text = selectedFile;
            myTreeModel.setTreeData(fileText.text);
        }
    }

    FluContentDialog{
        id: failedVariDialog
        title: qsTr("Failed Vari")
        message: qsTr("Please delete them or manually load them.")
        buttonFlags: FluContentDialogType.PositiveButton
        property var failedList:null
        contentDelegate: Component{
            Item{
                width: parent.width
                implicitHeight: failedVariView.height
                ListView {
                    id: failedVariView
                    width: 300
                    height: 200
                    model: failedVariDialog.failedList
                    clip: true
                    delegate: FluText {
                        anchors.centerIn: parent
                        text: (index+1)+": "+modelData
                    }
                }
            }
        }
        function openList(list) {
            failedVariDialog.failedList = list
            open()
        }
    }

    function openAppendNode(f) {
        sheet.appendAction = f;
        open(FluSheetType.Bottom)
    }

    function reloadFile() {
        myTreeModel.setTreeData(fileText.text);
        let variList = Backend.reloadVari()
        console.log(variList)
        if (variList.length!==0) {
            failedVariDialog.openList(variList)
            return false
        } else {
            showSuccess("Successfully reloaded")
            return true
        }
    }

    function setFileTip() {
        fileText.text = myTreeModel.getFileNameUrl()
        sheet.reloadFile()
    }

    Component.onCompleted: {
        console.log("variWin open")
    }
    Component.onDestruction: {
        console.log("variWin close")
    }
}

