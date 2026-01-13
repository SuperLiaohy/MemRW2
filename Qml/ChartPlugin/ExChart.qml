import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    clip:true

    signal logFileErrorHappen()

    // expose
    property alias lineAttrModel: chartView.lineAttrModel
    property alias targetFps: chartView.targetFps
    property alias realFps: chartView.realFps
    property alias viewXMax: chartView.viewXMax
    property alias viewXMin: chartView.viewXMin
    property alias viewYMin: chartView.viewYMin
    property alias viewYMax: chartView.viewYMax
    property alias flow: chartView.flow
    property alias log: chartView.log
    property alias logFile: chartView.logFile
    property alias tipsVisible: tipsLine.visible
    property alias mouseInChart: hoverHandler.hovered
    property alias mouseXValue: hoverHandler.mouseXValue
    property alias mouseYValue: hoverHandler.mouseYValue

    // need
    required property bool running

    // internal vari
    property int xGrid: 5
    property int yGrid: 5
    /* === y axis === */
    Item {
        id: yAxisLabels
        anchors.left: parent.left
        anchors.top: chartArea.top
        anchors.bottom: chartArea.bottom
        width: 40
        FluRectangle {
            anchors.fill:parent
            color: FluColors.Blue.normal
            opacity: 0.2
            radius: [5,0,0,5]
        }
        Repeater {
            anchors.fill: parent
            id: yRep
            model: frame.yGrid
            FluText {
                // height: parent.height / (yRep.count-1)
                x:Math.max(0,parent.width-width/2)
                y: index===(yRep.count-1)? parent.height-height:index*parent.height/(yRep.count-1)
                text: (chartView.viewYMax - (index / (yRep.count-1)) * (chartView.viewYMax - chartView.viewYMin)).toFixed(3)
                font.pixelSize: dragYHandler.active||dragHandler.active?15:10
                font.bold: dragYHandler.active||dragHandler.active
                color: FluTheme.dark ? "#888" : "#666"
                horizontalAlignment: Text.AlignLeft
                rightPadding: 5
            }
        }
        DragHandler {
            id: dragYHandler
            target: null
            cursorShape: Qt.ClosedHandCursor
            grabPermissions: PointerHandler.TakeOverForbidden
            onTranslationChanged: (delta) => {
                const Range = chartView.viewYMax - chartView.viewYMin
                const deltaValue = (delta.y) / parent.height * Range
                chartView.viewYMin += deltaValue
                chartView.viewYMax += deltaValue
                chartView.update()
            }
        }
        WheelHandler {
            id: wheelYHandler
            onWheel: (event) => {
                const factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                const yRange = chartView.viewYMax - chartView.viewYMin
                const yCenter = chartView.viewYMax - point.position.y / parent.height * yRange
                chartView.viewYMax = yCenter + point.position.y / parent.height * (yRange * factor)
                chartView.viewYMin = yCenter - (parent.height - point.position.y) / parent.height * (yRange * factor)
                chartView.update()
                event.accept = true
            }
        }
    }
    /* === chart === */
    Rectangle {
        id: chartArea
        anchors.left: yAxisLabels.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: xAxisLabels.top
        color: "transparent"
        Repeater {
            id: yGridLines
            model:  frame.yGrid - 2
            Rectangle {
                x: 0; y: (index + 1) * (chartView.height / (yGridLines.count + 1))
                width: chartView.width; height: 1
                color: FluTheme.dark ? "#666" : "#999"
                opacity:0.3
            }
        }
        Repeater {
            id: xGridLines
            model:  frame.xGrid - 2
            Rectangle {
                x: (index + 1) * (chartView.width / (xGridLines.count + 1)); y: 0
                width: 1; height: chartView.height
                color: FluTheme.dark ? "#666" : "#999"
                opacity:0.3
            }
        }
        ExChartView {
            id: chartView
            anchors.fill: parent
            onLogFileErrorHappen: {
                frame.logFileErrorHappen()
            }
        }
        Item {
            id: tipsLine
            anchors.fill: chartView
            Rectangle {
                x: hoverHandler.point.position.x; y: 0
                width: 2; height: chartView.height
                color: "blue"
                opacity:0.2
            }
            ListView {
                id: tipsInfo
                model: chartView.lineAttrModel
                anchors.fill: parent
                property var points: chartView.findPoints(hoverHandler.mouseXValue)
                delegate : Item{
                    id: controlItem
                    visible: model.visible
                    anchors.fill: parent
                    property bool valid: (index < tipsInfo.points.length && index>=0)
                    Rectangle {
                        id: tipPoint
                        width: 10
                        height: width
                        radius: width / 2
                        color: model.color
                        x: controlItem.valid ? tipsInfo.points[index].x - width/2 : 0
                        y: controlItem.valid ? tipsInfo.points[index].y - height/2 : 0
                    }
                    Item {
                        x: hoverHandler.point.position.x + 25
                        y: hoverHandler.point.position.y + index*25
                        Row {
                            Rectangle {
                                width: 4
                                height: 16
                                color: model.color
                                radius: 2
                            }
                            FluText {
                                leftPadding: 8
                                anchors.verticalCenter: parent.verticalCenter
                                text: model.name + ": " + (controlItem.valid?(chartView.viewYMax-tipsInfo.points[index].y/chartView.height*(chartView.viewYMax-chartView.viewYMin)).toFixed(3):0)
                                font.pixelSize: 10
                                font.bold: true
                                color: FluTheme.dark ?  "#ddd" : "#333"
                            }
                        }
                    }
                }
            }
        }

        Item {
            anchors.fill: chartView
            width: chartView.width
            height: chartView.height
            HoverHandler {
                id: hoverHandler
                acceptedDevices: PointerDevice.Mouse
                property real mouseXValue
                property real mouseYValue
                onPointChanged: {
                    let pt = pointToChart(point.position.x, point.position.y)
                    mouseXValue = pt.x
                    mouseYValue = pt.y
                }
            }
            TapHandler{
                onDoubleTapped: {
                    const yRange = chartView.viewYMax - chartView.viewYMin
                    const yCenter = chartView.viewYMax - point.position.y / parent.height * yRange
                    chartView.viewYMax = yCenter + point.position.y / parent.height * chartView.viewYRange
                    chartView.viewYMin = yCenter - (parent.height - point.position.y) / parent.height * chartView.viewYRange
                    const xRange = chartView.viewXMax - chartView.viewXMin
                    const xCenter = chartView.viewXMin + point.position.x / parent.width * xRange
                    chartView.viewXMax = xCenter + (parent.width - point.position.x) / parent.width * chartView.viewXRange
                    chartView.viewXMin = xCenter - point.position.x / parent.width * chartView.viewXRange
                    chartView.update()
                }
            }
            DragHandler {
                id: dragHandler
                target: null
                cursorShape: Qt.ClosedHandCursor
                grabPermissions: PointerHandler.TakeOverForbidden
                enabled: !(chartView.flow&&frame.running)
                onTranslationChanged: (delta) => {
                    var Range = chartView.viewYMax - chartView.viewYMin
                    var deltaValue = (delta.y) / parent.height * Range
                    chartView.viewYMin += deltaValue
                    chartView.viewYMax += deltaValue

                    Range = chartView.viewXMax - chartView.viewXMin
                    deltaValue = -(delta.x) / parent.width * Range
                    chartView.viewXMin += deltaValue
                    chartView.viewXMax += deltaValue

                    chartView.update()
                }
            }
            WheelHandler {
                id: wheelHandler
                enabled: !(chartView.flow&&frame.running)
                onWheel: (event) => {
                    const factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                             console.log(event.angleDelta.y)
                    const hasCtrl  = (event.modifiers & Qt.ControlModifier) !== 0
                    const hasShift = (event.modifiers & Qt.ShiftModifier)   !== 0
                    if(!hasCtrl) {
                        const yRange = chartView.viewYMax - chartView.viewYMin
                        const yCenter = chartView.viewYMax - point.position.y / parent.height * yRange
                        chartView.viewYMax = yCenter + point.position.y / parent.height * (yRange * factor)
                        chartView.viewYMin = yCenter - (parent.height - point.position.y) / parent.height * (yRange * factor)
                    }
                    if (!hasShift) {
                        const xRange = chartView.viewXMax - chartView.viewXMin
                        const xCenter = chartView.viewXMin + point.position.x / parent.width * xRange
                        chartView.viewXMax = xCenter + (parent.width - point.position.x) / parent.width * (xRange * factor)
                        chartView.viewXMin = xCenter - point.position.x / parent.width * (xRange * factor)
                    }
                    chartView.update()
                    event.accept = true
                }
            }
        }
    }
    /* === x axis === */
    Item {
        id: xAxisLabels
        anchors.left: chartArea.left
        anchors.right: chartArea.right
        anchors.bottom: parent.bottom
        height: 40
        FluRectangle {
            anchors.fill:parent
            color: FluColors.Blue.normal
            opacity: 0.2
            radius: [5,0,0,5]
        }
        Repeater {
            anchors.fill: parent
            id: xRep
            model: frame.xGrid
            FluText {
                height: parent.height
                x: index===(xRep.count-1)? parent.width-width:index*parent.width/(xRep.count-1)
                text: (chartView.viewXMin + (index / (xRep.count-1)) * (chartView.viewXMax - chartView.viewXMin)).toFixed(3)
                font.pixelSize: dragXHandler.active||dragHandler.active?15:10
                font.bold: dragXHandler.active||dragHandler.active
                color: FluTheme.dark ? "#888" : "#666"
                horizontalAlignment: Text.AlignLeft
            }
        }
        DragHandler {
            id: dragXHandler
            target: null
            cursorShape: Qt.ClosedHandCursor
            grabPermissions: PointerHandler.TakeOverForbidden
            enabled: !(chartView.flow&&frame.running)
            onTranslationChanged: (delta) => {
                const Range = chartView.viewXMax - chartView.viewXMin
                const deltaValue = -(delta.x) / parent.width * Range
                chartView.viewXMin += deltaValue
                chartView.viewXMax += deltaValue
                chartView.update()
            }
        }
        WheelHandler {
            id: wheelXHandler
            enabled: !(chartView.flow&&frame.running)
            onWheel: (event) => {
                const factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                const xRange = chartView.viewXMax - chartView.viewXMin
                const xCenter = chartView.viewXMin + point.position.x / parent.width * xRange
                chartView.viewXMax = xCenter + (parent.width - point.position.x) / parent.width * (xRange * factor)
                chartView.viewXMin = xCenter - point.position.x / parent.width * (xRange * factor)
                chartView.update()
                event.accept = true
            }
        }
    }

    /* === setting === */
    FluIconButton {
        anchors.right: xAxisLabels.left
        anchors.left: parent.left
        anchors.top: yAxisLabels.bottom
        anchors.bottom: parent.bottom
        iconSource: FluentIcons.Settings
        iconSize: 15
        onClicked: {
            chartDialog.open()
        }
    }
    FluContentDialog{
        id: chartDialog
        title: qsTr("Chart Edit")
        width:250
        property real editXRange
        property real editXCenter
        property real editYRange
        property real editYCenter
        property int editXGrid
        property int editYGrid
        contentDelegate: Component{
            Item{
                width: parent.width
                implicitHeight: 230
                ColumnLayout {
                    anchors.fill: parent
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        FluText {
                            text: "XRange"
                        }
                        FluSpinBox {
                            editable: true
                            from: 0
                            to: 2147483647
                            stepSize: 1
                            value: chartView.viewXRange
                            onValueChanged:{chartDialog.editXRange=value}
                        }
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        FluText {
                            text: "XCenter"
                        }
                        FluSpinBox {
                            editable: true
                            from: -2147483647
                            to: 2147483647
                            stepSize: 1
                            value: chartView.viewXCenter
                            onValueChanged:{chartDialog.editXCenter=value}
                        }
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        FluText {
                            text: "YRange"
                        }
                        FluSpinBox {
                            editable: true
                            from: 0
                            to: 2147483647
                            stepSize: 1
                            value: chartView.viewYRange
                            onValueChanged:{chartDialog.editYRange=value}
                        }
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        FluText {
                            text: "YCenter"
                        }
                        FluSpinBox {
                            editable: true
                            from: -2147483647
                            to: 2147483647
                            stepSize: 1
                            value: chartView.viewYCenter
                            onValueChanged:{chartDialog.editYCenter=value}
                        }
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        FluText {text: "XGrid"}
                        FluSpinBox {
                            from: 2
                            to: 10
                            stepSize: 1
                            value: frame.xGrid
                            onValueChanged:{chartDialog.editXGrid=value}
                        }
                    }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true
                        FluText {
                            text: "YGrid"
                        }
                        FluSpinBox {
                            from: 2
                            to: 10
                            stepSize: 1
                            value: frame.yGrid
                            onValueChanged:{chartDialog.editYGrid=value}
                        }
                    }
                }
            }
        }

        onPositiveClicked: {
            Qt. callLater(function() {
                chartView.viewXRange = chartDialog.editXRange
                chartView.viewXCenter = chartDialog.editXCenter
                chartView.viewYRange = chartDialog.editYRange
                chartView.viewYCenter = chartDialog.editYCenter
                frame.xGrid = chartDialog.editXGrid
                frame.yGrid = chartDialog.editYGrid
                showSuccess(qsTr("Save Settings"))
            })
        }
    }

    function resetChart() {
        chartView.viewXMax = chartView.viewXCenter + chartView.viewXRange / 2
        chartView.viewXMin = chartView.viewXCenter - chartView.viewXRange / 2
        chartView.viewYMax = chartView.viewYCenter + chartView.viewYRange / 2
        chartView.viewYMin = chartView.viewYCenter - chartView.viewYRange / 2
        chartView.update()
    }
    function pointToChart(xm,ym) {
        return {x:chartView.viewXMin+xm/chartView.width*(chartView.viewXMax-chartView.viewXMin),
            y:chartView.viewYMax-ym/chartView.height*(chartView.viewYMax-chartView.viewYMin)}
    }
}
