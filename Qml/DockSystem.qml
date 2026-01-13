import QtQuick
import QtQuick.Window
import QtQuick.Controls
import FluentUI

Item {
    id: root

    // ===== 对外接口 =====
    property string panel1Title: "面板 1"
    property Component panel1Content: null

    property string panel2Title: "面板 2"
    property Component panel2Content: null

    // ===== 内部状态 =====
    property bool panel1Docked: true
    property bool panel2Docked:  true
    property int currentTab: 0
    property string layoutMode: "tabbed"

    property bool isDragging: false
    property string draggingTitle: ""
    property real dragX: 0
    property real dragY:  0

    property real splitRatio: 0.5

    readonly property int dockedCount: (panel1Docked ? 1 :  0) + (panel2Docked ? 1 :  0)

    // 拖拽预览
    Loader {
        active: root.isDragging && root.dragX > 0
        sourceComponent: Window {
            x: root.dragX - 75
            y: root.dragY - 20
            width: 150
            height:  40
            flags: Qt. ToolTip | Qt. FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.WindowTransparentForInput
            visible: true
            color: "transparent"

            Rectangle {
                anchors.fill: parent
                color: FluTheme.primaryColor
                radius: 6
                opacity: 0.9
                Text {
                    anchors.centerIn: parent
                    text: root.draggingTitle
                    color:  "#fff"
                    font.pixelSize: 12
                    font.bold: true
                }
            }

            Binding on x { value: root.dragX - 75 }
            Binding on y { value:  root.dragY - 20 }
        }
    }

    // ===== 两个 Panel 实例 =====
    Item {
        id:  panel1Instance
        visible: false
        Loader {
            anchors.fill: parent
            sourceComponent: root.panel1Content
        }
    }

    Item {
        id: panel2Instance
        visible: false
        Loader {
            anchors.fill: parent
            sourceComponent: root.panel2Content
        }
    }

    // ===== 两个浮动窗口 =====
    Window {
        id:  floatWindow1
        width: 800
        height: 500
        visible: ! root.panel1Docked
        title: root.panel1Title
        color: FluTheme.dark ? "#2a2a2a" : "#f5f5f5"

        onClosing: function(close) {
            close.accepted = false
            root. dockPanel(0)
        }

        FluFrame {
            anchors.fill: parent
            anchors.margins: 8

            Rectangle {
                width: 28
                height: 28
                radius: 4
                anchors.right: parent.right
                anchors.top: parent.top
                color: dockBtn1.containsMouse ? (FluTheme. dark ? "#4a4a4a" : "#d0d0d0") : "transparent"

                Text {
                    anchors.centerIn: parent
                    text: "⇲"
                    color: FluTheme.dark ? "#fff" :  "#333"
                    font.pixelSize: 14
                }

                MouseArea {
                    id: dockBtn1
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt. PointingHandCursor
                    onClicked: root.dockPanel(0)
                }

                ToolTip. visible: dockBtn1.containsMouse
                ToolTip.text: "停靠到主窗口"
                ToolTip. delay: 500
            }

            Item {
                id:  floatContent1
                anchors.fill: parent
                anchors.topMargin: 36
            }
        }
    }

    Window {
        id:  floatWindow2
        width: 500
        height: 400
        visible:  !root.panel2Docked
        title: root.panel2Title
        color: FluTheme.dark ? "#2a2a2a" : "#f5f5f5"

        onClosing: function(close) {
            close. accepted = false
            root.dockPanel(1)
        }

        FluFrame {
            anchors.fill: parent
            anchors. margins: 8

            Rectangle {
                width: 28
                height: 28
                radius: 4
                anchors.right: parent. right
                anchors.top: parent. top
                color:  dockBtn2.containsMouse ? (FluTheme. dark ? "#4a4a4a" : "#d0d0d0") : "transparent"

                Text {
                    anchors.centerIn: parent
                    text: "⇲"
                    color: FluTheme.dark ? "#fff" : "#333"
                    font.pixelSize: 14
                }

                MouseArea {
                    id:  dockBtn2
                    anchors. fill: parent
                    hoverEnabled:  true
                    cursorShape: Qt. PointingHandCursor
                    onClicked: root. dockPanel(1)
                }

                ToolTip.visible: dockBtn2.containsMouse
                ToolTip.text: "停靠到主窗口"
                ToolTip.delay: 500
            }

            Item {
                id: floatContent2
                anchors.fill: parent
                anchors.topMargin: 36
            }
        }
    }

    // ===== 主界面 =====
    FluFrame {
        anchors.fill: parent

        // Tab 栏
        Item {
            id:  tabBar
            width: parent.width
            height: 40

            Row {
                anchors.left: parent.left
                anchors.leftMargin: 8
                anchors. verticalCenter: parent. verticalCenter
                spacing: 4

                TabButton {
                    visible: root.panel1Docked
                    title: root.panel1Title
                    isActive: root.currentTab === 0 || ! root.panel2Docked
                    onClicked:  root.currentTab = 0
                    onDragStarted: function(gx, gy) { startDrag(0, root.panel1Title, gx, gy) }
                    onDragMoved: function(gx, gy) { updateDrag(gx, gy) }
                    onDragEnded:  function(gx, gy) { endDrag(0, gx, gy) }
                }

                TabButton {
                    visible: root.panel2Docked
                    title: root.panel2Title
                    isActive: root.currentTab === 1 || !root.panel1Docked
                    onClicked: root.currentTab = 1
                    onDragStarted: function(gx, gy) { startDrag(1, root.panel2Title, gx, gy) }
                    onDragMoved: function(gx, gy) { updateDrag(gx, gy) }
                    onDragEnded: function(gx, gy) { endDrag(1, gx, gy) }
                }
            }

            // 布局按钮
            Row {
                anchors.right: parent.right
                anchors.rightMargin: 8
                anchors. verticalCenter: parent.verticalCenter
                spacing: 4

                Repeater {
                    model: [
                        { mode: "tabbed", icon: "▣" },
                        { mode: "horizontal", icon: "◫" },
                        { mode: "vertical", icon: "⬓" }
                    ]

                    Rectangle {
                        width: 28
                        height:  28
                        radius: 4
                        color: root.layoutMode === modelData.mode ? FluTheme.primaryColor :
                            (layoutBtnArea.containsMouse ?  (FluTheme. dark ? "#4a4a4a" : "#d0d0d0") : "transparent")

                        Text {
                            anchors.centerIn: parent
                            text: modelData.icon
                            color: root.layoutMode === modelData.mode ? "#fff" : (FluTheme. dark ? "#fff" : "#333")
                            font.pixelSize: 14
                        }

                        MouseArea {
                            id: layoutBtnArea
                            anchors.fill: parent
                            hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: root.layoutMode = modelData.mode
                        }
                    }
                }
            }
        }

        // 内容区域
        Item {
            id: contentArea
            anchors.top: tabBar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 8
            anchors. topMargin: 4

            // 空状态
            Rectangle {
                anchors.fill: parent
                color: FluTheme.dark ? "#1e1e1e" : "#e0e0e0"
                radius: 4
                border.color: FluTheme.dark ?  "#444" : "#ddd"
                border.width: 1
                visible: root. dockedCount === 0

                Text {
                    anchors. centerIn: parent
                    text: "所有面板已浮动"
                    color: FluTheme.dark ?  "#888" : "#666"
                    font.pixelSize: 14
                }
            }

            // 标签页模式
            Item {
                anchors.fill: parent
                visible: root.layoutMode === "tabbed" && root.dockedCount > 0

                PanelContainer {
                    id:  tabbedContainer1
                    anchors.fill: parent
                    visible: root.panel1Docked && (root.currentTab === 0 || ! root.panel2Docked)
                }

                PanelContainer {
                    id: tabbedContainer2
                    anchors.fill: parent
                    visible: root.panel2Docked && (root.currentTab === 1 || ! root.panel1Docked)
                }
            }

            // 水平并列
            Item {
                anchors.fill: parent
                visible:  root.layoutMode === "horizontal" && root.dockedCount > 0

                PanelContainer {
                    id: horizContainer1
                    visible: root.panel1Docked
                    anchors. left: parent.left
                    anchors.top: parent.top
                    anchors. bottom: parent.bottom
                    width: root. dockedCount === 2 ? parent.width * root.splitRatio - 3 : parent.width
                    panelTitle: root. panel1Title
                    showTitle: true
                }

                // 水平拖拽条
                DragHandle {
                    id: horizHandle
                    visible: root. dockedCount === 2
                    anchors.top: parent.top
                    anchors. bottom: parent.bottom
                    x: parent.width * root.splitRatio - 3
                    width: 6
                    orientation: Qt.Horizontal

                    onDragMove: function(delta) {
                        var newRatio = root.splitRatio + delta / contentArea.width
                        root.splitRatio = Math.max(0.2, Math.min(0.8, newRatio))
                    }
                }

                PanelContainer {
                    id: horizContainer2
                    visible: root.panel2Docked
                    anchors.right: parent.right
                    anchors. top: parent.top
                    anchors.bottom: parent. bottom
                    width: root.dockedCount === 2 ? parent.width * (1 - root. splitRatio) - 3 : parent.width
                    panelTitle: root.panel2Title
                    showTitle:  true
                }
            }

            // 垂直并列
            Item {
                anchors.fill: parent
                visible: root.layoutMode === "vertical" && root. dockedCount > 0

                PanelContainer {
                    id: vertContainer1
                    visible: root.panel1Docked
                    anchors.left: parent.left
                    anchors. right: parent.right
                    anchors. top: parent.top
                    height:  root.dockedCount === 2 ?  parent.height * root.splitRatio - 3 : parent.height
                    panelTitle: root.panel1Title
                    showTitle:  true
                }

                // 垂直拖拽条
                DragHandle {
                    id:  vertHandle
                    visible: root. dockedCount === 2
                    anchors.left: parent.left
                    anchors. right: parent.right
                    y: parent.height * root.splitRatio - 3
                    height: 6
                    orientation:  Qt.Vertical

                    onDragMove: function(delta) {
                        var newRatio = root.splitRatio + delta / contentArea.height
                        root.splitRatio = Math.max(0.2, Math.min(0.8, newRatio))
                    }
                }

                PanelContainer {
                    id:  vertContainer2
                    visible: root. panel2Docked
                    anchors.left: parent.left
                    anchors. right: parent.right
                    anchors.bottom: parent.bottom
                    height: root.dockedCount === 2 ? parent.height * (1 - root.splitRatio) - 3 : parent. height
                    panelTitle: root. panel2Title
                    showTitle: true
                }
            }
        }
    }

    // ===== 函数 =====

    function startDrag(panel, title, gx, gy) {
        dragX = gx
        dragY = gy
        draggingTitle = title
        isDragging = true
    }

    function updateDrag(gx, gy) {
        dragX = gx
        dragY = gy
    }

    function endDrag(panel, gx, gy) {
        isDragging = false
        draggingTitle = ""
        dragX = 0
        dragY = 0
        floatPanel(panel, gx - 200, gy - 20)
    }

    function floatPanel(panel, x, y) {
        if (panel === 0 && panel1Docked) {
            floatWindow1.x = x
            floatWindow1.y = y
            panel1Docked = false
        } else if (panel === 1 && panel2Docked) {
            floatWindow2.x = x
            floatWindow2.y = y
            panel2Docked = false
        }

        if (! panel1Docked && panel2Docked) currentTab = 1
        else if (panel1Docked && !panel2Docked) currentTab = 0
    }

    function dockPanel(panel) {
        if (panel === 0) {
            panel1Docked = true
            currentTab = 0
        } else if (panel === 1) {
            panel2Docked = true
            currentTab = 1
        }
    }

    function getPanel1Container() {
        if (layoutMode === "tabbed") return tabbedContainer1
        if (layoutMode === "horizontal") return horizContainer1
        if (layoutMode === "vertical") return vertContainer1
        return null
    }

    function getPanel2Container() {
        if (layoutMode === "tabbed") return tabbedContainer2
        if (layoutMode === "horizontal") return horizContainer2
        if (layoutMode === "vertical") return vertContainer2
        return null
    }

    function updatePanelParent() {
        if (panel1Docked) {
            var c1 = getPanel1Container()
            if (c1) {
                panel1Instance.parent = c1.contentItem
                panel1Instance.anchors.fill = c1.contentItem
                panel1Instance. visible = true
            }
        } else {
            panel1Instance.parent = floatContent1
            panel1Instance. anchors.fill = floatContent1
            panel1Instance. visible = true
        }

        if (panel2Docked) {
            var c2 = getPanel2Container()
            if (c2) {
                panel2Instance. parent = c2.contentItem
                panel2Instance.anchors.fill = c2.contentItem
                panel2Instance.visible = true
            }
        } else {
            panel2Instance.parent = floatContent2
            panel2Instance.anchors.fill = floatContent2
            panel2Instance.visible = true
        }
    }

    onLayoutModeChanged: updatePanelParent()
    onPanel1DockedChanged: updatePanelParent()
    onPanel2DockedChanged: updatePanelParent()
    Component.onCompleted: updatePanelParent()

    // ===== 子组件 =====

    component TabButton:  Rectangle {
        id:  tabBtn
        width: Math.max(80, tabText.implicitWidth + 24)
        height: 28
        radius: 4
        color: isActive ? (FluTheme. dark ? "#3a3a3a" : "#e8e8e8") :
            (tabArea.containsMouse ?  (FluTheme.dark ? "#4a4a4a" : "#d0d0d0") : (FluTheme. dark ? "#2e2e2e" : "#f0f0f0"))
        border.color: isActive ? FluTheme.primaryColor : (FluTheme.dark ? "#444" :  "#ddd")
        border.width: isActive ? 1 : 0

        property string title: ""
        property bool isActive: false
            signal clicked
        signal dragStarted(real gx, real gy)
        signal dragMoved(real gx, real gy)
        signal dragEnded(real gx, real gy)

        Text {
            id:  tabText
            anchors.centerIn: parent
            text:  tabBtn.title
            color: tabBtn.isActive ?  (FluTheme.dark ? "#fff" : "#333") : (FluTheme.dark ? "#888" : "#666")
            font.pixelSize: 12
        }

        MouseArea {
            id:  tabArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt. PointingHandCursor
            property point startPos
            property bool dragging: false

            onPressed: function(e) {
                startPos = Qt.point(e. x, e.y)
                dragging = false
            }

            onPositionChanged: function(e) {
                if (! pressed) return
                var d = Math.sqrt(Math.pow(e.x - startPos.x, 2) + Math.pow(e.y - startPos.y, 2))
                var gp = mapToGlobal(e.x, e.y)
                if (d > 20 && !dragging) {
                    dragging = true
                    tabBtn.dragStarted(gp. x, gp. y)
                }
                if (dragging) tabBtn.dragMoved(gp. x, gp. y)
            }

            onReleased: function(e) {
                if (dragging) {
                    var gp = mapToGlobal(e. x, e.y)
                    tabBtn.dragEnded(gp.x, gp.y)
                } else {
                    tabBtn.clicked()
                }
                dragging = false
            }
        }
    }

    component PanelContainer: Item {
        property string panelTitle:  ""
        property bool showTitle: false
        property alias contentItem: contentArea

        Rectangle {
            anchors.fill: parent
            color: FluTheme.dark ? "#2a2a2a" : "#f0f0f0"
            radius: 4
            border. color: FluTheme.dark ? "#444" : "#ddd"
            border.width: 1
        }

        Rectangle {
            id: titleRect
            width:  parent.width
            height: showTitle ?  32 : 0
            visible: showTitle
            color:  FluTheme. dark ? "#323232" : "#ebebeb"
            radius: 4

            Rectangle {
                anchors.bottom: parent. bottom
                width:  parent.width
                height: 4
                color: parent.color
            }

            Text {
                anchors.centerIn: parent
                text: panelTitle
                color: FluTheme.dark ? "#fff" :  "#333"
                font.pixelSize: 12
                font.bold: true
            }
        }

        Item {
            id: contentArea
            anchors.top: titleRect.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 6
            anchors. topMargin: showTitle ? 4 : 6
        }
    }

    component DragHandle: Rectangle {
        id: handle
        color: handleArea.containsMouse || handleArea.pressed ? FluTheme.primaryColor : (FluTheme. dark ? "#555" : "#ccc")
        radius: 2

        property int orientation: Qt.Horizontal
        signal dragMove(real delta)

        Rectangle {
            anchors.centerIn: parent
            width: handle.orientation === Qt.Horizontal ?  2 : 20
            height: handle.orientation === Qt. Horizontal ? 20 : 2
            radius: 1
            color: FluTheme.dark ? "#888" : "#999"
        }

        MouseArea {
            id: handleArea
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: handle.orientation === Qt. Horizontal ? Qt. SplitHCursor : Qt.SplitVCursor

            property real startPos: 0

            onPressed: function(e) {
                startPos = handle.orientation === Qt.Horizontal ? e.x : e.y
            }

            onPositionChanged: function(e) {
                if (!pressed) return
                var currentPos = handle.orientation === Qt.Horizontal ? e.x :  e.y
                var delta = currentPos - startPos
                handle.dragMove(delta)
            }
        }
    }
}
