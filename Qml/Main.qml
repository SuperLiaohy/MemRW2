import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQml
import FluentUI
import MemRWApp
FluWindow {
    id:window
    title: "Vari"
    width: 1200
    height: 480
    visible: true
    showStayTop: true
    fitsAppBarWindows: true

    appBar: FluAppBar {
        id: bar
        height: 30
        showDark: true
        darkClickListener:(button)=>handleDarkChanged(button)
        z:7
    }

    ColumnLayout {
        anchors.top: parent.top
        anchors.topMargin: bar.height
        anchors.fill: parent

        StateBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
        }

        // ==================== 主内容区 ====================
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8
            // ==================== 曲线列表侧边栏 ====================
            LineListView {
                Layout.preferredWidth: 220
                Layout.fillHeight: true
                lineAttrModel: chart.lineAttrModel
            }
            // ==================== 图表区域 ====================
            GridLayout {
                columns:2
                rows:2
                Layout.fillWidth: true
                Layout.fillHeight: true
                // ==================== Y轴选择器 ====================
                ValueAxisControlBar {
                    id: yBar
                    Layout.row: 0
                    Layout.column: 0
                    Layout.preferredWidth: 50
                    Layout.fillHeight: true
                }
                // ==================== 图表主体 ====================
                ExChart {
                    id: chart
                    Layout.row: 0
                    Layout.column: 1
                    Layout. fillWidth: true
                    Layout.fillHeight: true
                }
                // ⭐ ==================== X轴时间范围选择器 ====================
                TimeAxisControlBar {
                    id: xBar
                    Layout.row: 1
                    Layout.column: 0
                    Layout.columnSpan:2
                    Layout.fillWidth: true
                    Layout.preferredHeight: 60
                }
            }
        }
        // ==================== 控制栏 ====================
        ControlBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            padding: 10

        }
    }


    onHeightChanged: {
        if (sheet.height>sheet.maxSize)
            sheet.height = sheet.maxSize
    }
    VariWindow {
        id:sheet
        size: parent.height*0.614
        maxSize: parent.height
    }
    Loader {
        id: dialogLoader
        active: false
        sourceComponent: LineDialog {
            running: false
            visible: true
            onClosing: {
                visible = false
                console.log("Dialog closing")
                lineAttrModel = null
                dialogLoader.close()
            }
        }
        function open(model,object, openMode) {
            active = true;
            dialogLoader.item.open(model,object, openMode)
        }
        function close() {
            active = false;
        }
    }

    // ==================== 动画 ====================
    Component{
        id: com_reveal
        CircularReveal{
            id: reveal
            target: window.containerItem()
            anchors.fill: parent
            darkToLight: FluTheme.dark
            onAnimationFinished:{
                //动画结束后释放资源
                loader_reveal.sourceComponent = undefined
            }
            onImageChanged: {
                changeDark()
            }
        }
    }

    FluLoader{
        id:loader_reveal
        anchors.fill: parent
    }

    function distance(x1,y1,x2,y2){
        return Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))
    }
    function handleDarkChanged(button){
        if(FluTools.isMacos() || !FluTheme.animationEnabled){
            changeDark()
        }else{
            loader_reveal.sourceComponent = com_reveal
            var target = window.containerItem()
            var pos = button.mapToItem(target,0,0)
            var mouseX = pos.x + button.width / 2
            var mouseY = pos.y + button.height / 2
            var radius = Math.max(distance(mouseX,mouseY,0,0),distance(mouseX,mouseY,target.width,0),distance(mouseX,mouseY,0,target.height),distance(mouseX,mouseY,target.width,target.height))
            var reveal = loader_reveal.item
            reveal.start(reveal.width*Screen.devicePixelRatio,reveal.height*Screen.devicePixelRatio,Qt.point(mouseX,mouseY),radius)
        }
    }

    function changeDark(){
        if(FluTheme.dark){
            FluTheme.darkMode = FluThemeType.Light
        }else{
            FluTheme.darkMode = FluThemeType.Dark
        }
    }

}