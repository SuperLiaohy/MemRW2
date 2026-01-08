import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    clip:true

    // expose
    property alias lineAttrModel: chartView.lineAttrModel
    property alias viewXMax: chartView.viewXMax
    property alias viewXMin: chartView.viewXMin
    property alias viewYMin: chartView.viewYMin
    property alias viewYMax: chartView.viewYMax

    // internal variable
    property real dragStartY: 0
    property real dragStartYMin: 0
    property real dragStartYMax: 0

    property real dragStartX: 0
    property real dragStartXMin: 0
    property real dragStartXMax: 0

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
            model: 5
            FluText {
                // height: parent.height / (yRep.count-1)
                x:Math.max(0,parent.width-width/2)
                y: index===(yRep.count-1)? parent.height-height:index*parent.height/(yRep.count-1)
                text: (chartView.viewYMax - (index / (yRep.count-1)) * (chartView.viewYMax - chartView.viewYMin)).toFixed(1)
                font.pixelSize: yMouseArea.pressed||chartMouseArea.pressed?15:10
                font.bold: yMouseArea.pressed||chartMouseArea.pressed
                color: FluTheme.dark ? "#888" : "#666"
                horizontalAlignment: Text.AlignLeft
                rightPadding: 5
            }
        }
        MouseArea {
            id: yMouseArea
            anchors.fill: parent
            cursorShape: pressed?Qt.ClosedHandCursor:Qt.ArrowCursor
            onPressed: function(event) {
                frame.dragStartY = event.y
                frame.dragStartYMin = chartView.viewYMin
                frame.dragStartYMax = chartView.viewYMax
            }
            onPositionChanged: function(event) {
                if (pressed) {
                    var yRange = chartView.viewYMax - chartView.viewYMin
                    var deltaValue = (event.y - frame.dragStartY) / height * yRange
                    chartView.viewYMin = frame.dragStartYMin + deltaValue
                    chartView.viewYMax = frame.dragStartYMax + deltaValue
                    chartView.update()
                }
            }
            onWheel: function (event) {
                var factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                var yRange = chartView.viewYMax - chartView.viewYMin
                var center = chartView.viewYMax - event.y/height*yRange
                chartView.viewYMax = center + event.y/height* (yRange*factor)
                chartView.viewYMin = center - (height-event.y)/height*(yRange*factor)
                frame.dragStartY = event.y
                frame.dragStartYMin = chartView.viewYMin
                frame.dragStartYMax = chartView.viewYMax
                chartView.update()
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
            model:  3
            Rectangle {
                x: 0; y: (index + 1) * (chartView.height / 4)
                width: chartView.width; height: 1
                color: FluTheme.dark ? "#666" : "#999"
                opacity:0.2
            }
        }
        Repeater {
            model:  5
            Rectangle {
                x: (index + 1) * (chartView.width / 4); y: 0
                width: 1; height: chartView.height
                color: FluTheme.dark ? "#666" : "#999"
                opacity:0.2
            }
        }
        ExChartView {
            id: chartView
            anchors.fill: parent
        }
        MouseArea {
            id: chartMouseArea
            anchors.fill: parent
            cursorShape: pressed?Qt.ClosedHandCursor:Qt.ArrowCursor
            onDoubleClicked: function (event) {
                var yRange = chartView.viewYMax - chartView.viewYMin
                var yCenter = chartView.viewYMax - event.y / height * yRange
                chartView.viewYMax = yCenter + event.y / height * (chartView.viewYRange)
                chartView.viewYMin = yCenter - (height - event.y) / height * (chartView.viewYRange)

                var xRange = chartView.viewXMax - chartView.viewXMin
                var xCenter = chartView.viewXMin + event.x / width * xRange
                chartView.viewXMax = xCenter + (width - event.x) / width * (chartView.viewXRange)
                chartView.viewXMin = xCenter - event.x / width * (chartView.viewXRange)

                chartView.update()
            }
            onPressed: function(event) {
                frame.dragStartX = event.x
                frame.dragStartY = event.y
                frame.dragStartXMin = chartView.viewXMin
                frame.dragStartYMin = chartView.viewYMin
                frame.dragStartXMax = chartView.viewXMax
                frame.dragStartYMax = chartView.viewYMax
            }
            onPositionChanged: function(event) {
                if (pressed) {
                    var Range = chartView.viewYMax - chartView.viewYMin
                    var deltaValue = (event.y - frame.dragStartY) / height * Range
                    chartView.viewYMin = frame.dragStartYMin + deltaValue
                    chartView.viewYMax = frame.dragStartYMax + deltaValue

                    Range = chartView.viewXMax - chartView.viewXMin
                    deltaValue = -(event.x - frame.dragStartX) / width * Range
                    chartView.viewXMin = frame.dragStartXMin + deltaValue
                    chartView.viewXMax = frame.dragStartXMax + deltaValue

                    chartView.update()
                }
            }
            onWheel: function (event) {

                var factor = event.angleDelta.y > 0 ? 0.8 : 1.25

                var yRange = chartView.viewYMax - chartView.viewYMin
                var yCenter = chartView.viewYMax - event.y / height * yRange
                chartView.viewYMax = yCenter + event.y / height * (yRange * factor)
                chartView.viewYMin = yCenter - (height - event.y) / height * (yRange * factor)

                var xRange = chartView.viewXMax - chartView.viewXMin
                var xCenter = chartView.viewXMin + event.x / width * xRange
                chartView.viewXMax = xCenter + (width - event.x) / width * (xRange * factor)
                chartView.viewXMin = xCenter - event.x / width * (xRange * factor)

                frame.dragStartX = event.x
                frame.dragStartY = event.y
                frame.dragStartXMin = chartView.viewXMin
                frame.dragStartXMax = chartView.viewXMax
                frame.dragStartYMin = chartView.viewYMin
                frame.dragStartYMax = chartView.viewYMax
                chartView.update()

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
            model: 5
            FluText {
                height: parent.height
                x: index===(xRep.count-1)? parent.width-width:index*parent.width/(xRep.count-1)
                text: (chartView.viewXMin + (index / (xRep.count-1)) * (chartView.viewXMax - chartView.viewXMin)).toFixed(1)
                font.pixelSize: xMouseArea.pressed||chartMouseArea.pressed?15:10
                font.bold: xMouseArea.pressed||chartMouseArea.pressed
                color: FluTheme.dark ? "#888" : "#666"
                horizontalAlignment: Text.AlignLeft
            }
        }
        MouseArea {
            id: xMouseArea
            anchors.fill: parent
            cursorShape: pressed?Qt.ClosedHandCursor:Qt.ArrowCursor
            onPressed: function(event) {
                frame.dragStartX = event.x
                frame.dragStartXMin = chartView.viewXMin
                frame.dragStartXMax = chartView.viewXMax
            }
            onPositionChanged: function(event) {
                if (pressed) {
                    var xRange = chartView.viewXMax - chartView.viewXMin
                    var deltaValue = -(event.x - frame.dragStartX) / width * xRange
                    chartView.viewXMin = frame.dragStartXMin + deltaValue
                    chartView.viewXMax = frame.dragStartXMax + deltaValue
                    chartView.update()
                }
            }
            onWheel: function (event) {
                var factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                var xRange = chartView.viewXMax - chartView.viewXMin
                var center = chartView.viewXMin + event.x/width*xRange
                chartView.viewXMax = center + (width-event.x)/width*(xRange*factor)
                chartView.viewXMin = center - event.x/width* (xRange*factor)
                frame.dragStartX = event.x
                frame.dragStartXMin = chartView.viewXMin
                frame.dragStartXMax = chartView.viewXMax
                chartView.update()
            }
        }
    }
    FluIconButton {
        anchors.right: xAxisLabels.left
        anchors.left: parent.left
        anchors.top: yAxisLabels.bottom
        anchors.bottom: parent.bottom
        iconSource: FluentIcons.Settings
        iconSize: 15
    }
}
