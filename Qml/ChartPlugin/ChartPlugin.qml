import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: chartPlugin

    // ==================== 主内容区 ====================
    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        ChartBar {
            id: chartBar
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            running: Backend.running
            viewXMin: chart.viewXMin
            viewXMax: chart.viewXMax
            viewYMin: chart.viewYMin
            viewYMax: chart.viewYMax
            realFps: chart.realFps
            mouseInChart: chart.mouseInChart
            mouseXValue: chart.mouseXValue
            mouseYValue: chart.mouseYValue

            onResetClicked: chart.resetChart
        }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            LineListView {
                Layout.preferredWidth: 220
                Layout.fillHeight: true
                lineAttrModel: chart.lineAttrModel
                visible: chartBar.showLineList
                running: Backend.running
            }
            ExChart {
                id: chart
                Layout.fillWidth: true
                Layout.fillHeight: true
                targetFps: chartBar.targetFps
                flow: chartBar.flow
                tipsVisible: chartBar.showCrosshair
                running: Backend.running
            }
        }
    }
}