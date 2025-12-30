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
        anchors.top: chartView.top
        anchors.bottom: chartView.bottom
        width: 40
        FluRectangle {
            anchors.fill:parent
            color: "blue"
            opacity: 0.1
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
    }
    ExChartView {
        id: chartView
        anchors.left: yAxisLabels.right
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: xAxisLabels.top
    }
    Item {
        id: xAxisLabels
        anchors.left: chartView.left
        anchors.right: chartView.right
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 5
        height: 30
        FluRectangle {
            anchors.fill:parent
            color: "blue"
            opacity: 0.1
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
    }
}
