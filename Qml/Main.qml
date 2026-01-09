import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import FluentUI
import MemRWApp
FluWindow {
    id:window
    title: "Vari"
    width: 1100
    height: 700
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

        // ==================== 控制栏 ====================
        ToolBar {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            padding: 10
        }

        ChartPlugin {
            id: chartPlugin
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

    }


    onHeightChanged: {
        if (sheet.height>sheet.maxSize)
            sheet.height = sheet.maxSize
        console.log("height: ",height," width: ",width)

    }
    VariWindow {
        id:sheet
        size: parent.height*0.614
        maxSize: parent.height
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