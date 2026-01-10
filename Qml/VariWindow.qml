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

        MouseArea {
            anchors.fill: parent
            cursorShape: Qt.SizeVerCursor

            property real startY: 0
            property real startSize: sheet.height  // 用初始 size 或当前高度

            onPressed: (mouse) => {
                var point = mapToItem(null,mouse);
                startY = point.y
                startSize = sheet.height;
            }

            onPositionChanged: (mouse) => {
                var point = mapToItem(null,mouse);
                var delY = startY - point.y
                if (startSize+delY<sheet.maxSize)
                    sheet.height = startSize+delY
                else
                    sheet.height = maxSize
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
                fileDialog.open()
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
        TextField {
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
    function minItemWidth(column) {
        if (column===0) {
            return (customAdd.width)*0.65
        }
        if (column===1) {
            return (customAdd.width)*0.15
        }
        return (customAdd.width)*0.1
    }
    HorizontalHeaderView {
        id: header
        syncView: treeView
        anchors.top: customAdd.bottom
        anchors.left: customAdd.left
        anchors.right: customAdd.right
        clip: true
        resizableColumns: true
        width: parent.width
        onWidthChanged: forceLayout()
        delegate: Item {
            implicitWidth: minItemWidth(column)
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
        }
    }

    TreeView {
        id: treeView
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.left: customAdd.left
        anchors.right: customAdd.right
        model: myTreeModel
        clip: true
        selectionModel: ItemSelectionModel {
            id: selModel
            model: treeView.model
        }

        delegate: Item {
            implicitWidth: padding + label.x + label.minWidth + padding
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

            FluRectangle {
                id: background
                anchors.fill: parent
                color: row === treeView.currentRow ? palette.highlight : (FluTheme.dark ? "#2a2a2a" : "#f0f0f0")
                opacity: 0.3
                borderColor: "#145eef"
                borderWidth: 1
                radius: rad(column)

                function rad(column) {
                    if (column === 0) return [7, 0, 0, 7]
                    if (column === 3) return [0, 7, 7, 0]
                    return [0, 0, 0, 0]
                }
            }

            RowLayout {
                anchors.fill:parent
                Loader {
                    id: indicatorLoader
                    active: isTreeNode && hasChildren
                    anchors.verticalCenter: parent.verticalCenter
                    sourceComponent: FluIcon {
                        id: indicator
                        x: 5 + padding + (depth * indentation)
                        iconSource: expanded ? FluentIcons.ChevronDown : FluentIcons.ChevronRight
                        iconSize: 10
                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                let index = treeView.index(row, column)
                                treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
                                treeView.toggleExpanded(row)
                            }
                        }
                    }
                }

                FluText {
                    id: label
                    x: padding + (isTreeNode ? (depth + 1) * indentation : 0)
                    property int minWidth: minItemWidth(column)
                    width: Math.max(minWidth, parent.width - padding - x)
                    anchors.verticalCenter: parent.verticalCenter
                    clip: true
                    text: column === 0 ? smartElideLeft(model.display, width, fm) : model.display
                    FontMetrics {
                        id: fm; font: label.font
                    }

                    function smartElideLeft(path, maxWidth, fontMetrics) {
                        if (fontMetrics.boundingRect(path).width <= maxWidth) return path;
                        var parts = path.split('/');  // Linux/Mac 路径
                        // var parts = path.split('\\'); // Windows 路径，可根据需要判断
                        while (parts.length > 1) {
                            parts.shift();  // 移除最左侧的目录
                            var temp = ".../" + parts.join('/');
                            if (fontMetrics.boundingRect(temp).width <= maxWidth) return temp;
                        }
                        return ".../" + parts[0];  // 只剩最后一部分
                    }
                }

                Loader {
                    id: addBtn
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    active: !hasChildren && column === 0
                    sourceComponent: FluIconButton {
                        iconSource: FluentIcons.Add
                        onClicked: {
                            sheet.appendAction(model);
                        }
                    }
                }
            }
        }
    }

    FileDialog {
        id: fileDialog
        nameFilters: ["dwarf files [*.elf *.axf] (*.elf *.axf)", "all files [*.*] (*.*)"]

        onAccepted: {
            console.log("选择了:", selectedFile)
            console.log("选择了:", data)
            fileText.text = selectedFile;
            myTreeModel.setTreeData(selectedFile);
        }
    }
    function openAppendNode(f) {
        sheet.appendAction = f;
        open(FluSheetType.Bottom)
    }
}

