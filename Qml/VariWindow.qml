import QtCore
import QtQuick
import QtQuick.Controls.FluentWinUI3
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
        width: parent.width * 0.95
        height: 6
        radius: 3
        color: "#80000000"
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

    // visible: false
    ToolBar {
        id: toolBar
        FluButton {
            id: loadBtn
            anchors.verticalCenter: parent.verticalCenter
            text: "选择文件..."
            onClicked: {
                fileDialog.open()
            }
        }
        FluText {
            x: loadBtn.width + 20
            anchors.verticalCenter: parent.verticalCenter
            id: fileText
            text: "";
        }
    }

    function minItemWidth(column) {
        if (column===0) {
            return (sheet.width)/2
        }
        if (column===1) {
            return (sheet.width)/6
        }
        return (sheet.width)/12
    }
    HorizontalHeaderView {
        id: header
        syncView: treeView
        anchors.top: toolBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
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
        anchors.left: parent.left
        anchors.right: parent.right
        model: myTreeModel
        clip: true
        selectionModel: ItemSelectionModel {}

        delegate: Item {
            // implicitWidth: Math.max(
            //     padding + label.x + 200 + padding,   // 内容需求
            //     padding + label.x + label.minWidth + padding        // 最小宽度需求
            // )
            implicitWidth: padding + label.x + label.minWidth + padding
            implicitHeight: label.implicitHeight * 1.5

            readonly property real indentation: 20
            readonly property real padding: 5

            // Assigned to by TreeView:
            required property TreeView treeView
            required property bool isTreeNode
            required property bool expanded
            required property int hasChildren
            required property int depth
            required property int row
            required property int column
            required property bool current

            // Rotate indicator when expanded by the user
            // (requires TreeView to have a selectionModel)
            property Animation indicatorAnimation: NumberAnimation
            {
                target: indicatorLoader.item
                property: "rotation"
                from: expanded ? 0 : 90
                to: expanded ? 90 : 0
                duration: 100
                easing.type: Easing.OutQuart
            }
            TableView.onPooled: indicatorAnimation.complete()
            TableView.onReused: if (current) indicatorAnimation.start()
            onExpandedChanged: { if (indicatorLoader.item) indicatorLoader.item.rotation = expanded ? 90 : 0}

            FluRectangle {
                id: background
                anchors.fill: parent
                // color:
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

            Loader {
                id: indicatorLoader
                active: isTreeNode && hasChildren
                sourceComponent: FluText {
                    id: indicator
                    x: 5 + padding + (depth * indentation)
                    anchors.verticalCenter: parent.verticalCenter
                    // visible: isTreeNode && hasChildren
                    text: "▶"

                    TapHandler {
                        onSingleTapped: {
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
                // width: parent.width - padding - x
                property int minWidth: minItemWidth(column)
                width: Math.max(minWidth, parent.width - padding - x)
                anchors.verticalCenter: parent.verticalCenter
                clip: true
                text: column === 0 ? smartElideLeft(model.display, width, fm) : model.display
                // text: model.display

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
                active: !hasChildren && column===0
                sourceComponent: FluIconButton {
                    iconSource: FluentIcons.Add
                    onClicked: {
                        sheet.appendAction(model);
                        // lineModel.appendLine(model.display,"red")
                    }
                }
            }

        }
    }

    FileDialog {
        id: fileDialog
        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]
        nameFilters: ["dwarf files [*.elf *.axf] (*.elf *.axf)", "all files [*.*] (*.*)"]

        onAccepted: {
            console.log("选择了:", selectedFile)
            console.log("选择了:", data)
            fileText.text = selectedFile;
            myTreeModel.setTreeData(selectedFile);
        }
    }

    function openToAppend(f) {
        sheet.appendAction = f;
        open(FluSheetType.Bottom)
    }
}

