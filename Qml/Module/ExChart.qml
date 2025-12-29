import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {
    id: frame
    property alias lineAttrModel: chartView.lineAttrModel
    ExChartView {
        id: chartView
        anchors.fill: parent
    }
}
