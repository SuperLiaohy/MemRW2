import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    property alias lineAttrModel: chartView.lineAttrModel
    clip:true
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
                width: parent.width
                // height: parent.height / (yRep.count-1)
                y: index===(yRep.count-1)? parent.height-height:index*parent.height/(yRep.count-1)
                text: (chartView.viewYMax - (index / 4) * (chartView.viewYMax - chartView.viewYMin)).toFixed(1)
                font.pixelSize: 10
                color: FluTheme.dark ? "#888" : "#666"
                horizontalAlignment: Text.AlignRight
                rightPadding: 5
            }
        }
        MouseArea {
            anchors.fill: parent
            property real dragStartY: 0
            property real dragStartYMin: 0
            property real dragStartYMax: 0
            onPressed: function(event) {
                dragStartY = event.y
                dragStartYMin = chartView.viewYMin
                dragStartYMax = chartView.viewYMax
            }
            onPositionChanged: function(event) {
                if (pressed) {
                    var yRange = chartView.viewYMax - chartView.viewYMin
                    var deltaValue = (event.y - dragStartY) / height * yRange
                    chartView.viewYMin = dragStartYMin + deltaValue
                    chartView.viewYMax = dragStartYMax + deltaValue
                    chartView.update()
                }
            }
            onWheel: function (event) {
                var factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                var yRange = chartView.viewYMax - chartView.viewYMin
                var center = chartView.viewYMax - event.y/height*yRange
                chartView.viewYMax = center + event.y/height* (yRange*factor)
                chartView.viewYMin = center - (height-event.y)/height*(yRange*factor)
                chartView.update()
            }
        }
    }

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
    }
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
                text: (chartView.viewXMin + (index / 4) * (chartView.viewXMax - chartView.viewXMin)).toFixed(1)
                font.pixelSize: 10
                color: FluTheme.dark ? "#888" : "#666"
                horizontalAlignment: Text.AlignLeft
            }
        }
        MouseArea {
            anchors.fill: parent
            property real dragStartX: 0
            property real dragStartXMin: 0
            property real dragStartXMax: 0
            onPressed: function(event) {
                dragStartX = event.x
                dragStartXMin = chartView.viewXMin
                dragStartXMax = chartView.viewXMax
            }
            onPositionChanged: function(event) {
                if (pressed) {
                    var xRange = chartView.viewXMax - chartView.viewXMin
                    var deltaValue = -(event.x - dragStartX) / width * xRange
                    chartView.viewXMin = dragStartXMin + deltaValue
                    chartView.viewXMax = dragStartXMax + deltaValue
                    chartView.update()
                }
            }
            onWheel: function (event) {
                var factor = event.angleDelta.y > 0 ? 0.8 : 1.25
                var xRange = chartView.viewXMax - chartView.viewXMin
                var center = chartView.viewXMin + event.x/width*xRange
                chartView.viewXMax = center + (width-event.x)/width*(xRange*factor)
                chartView.viewXMin = center - event.x/width* (xRange*factor)
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
