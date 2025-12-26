import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {

    ListView {
        anchors.fill: parent
        model: LineGroupModel
        interactive: false
        delegate: ListView {
            anchors.fill: parent
            model: lineModel
            delegate: Item {
                anchors.fill: parent
                children: [model.object]
                Component.onCompleted: {
                    if (model.object) {
                        model.object.width = Qt.binding(() => width)
                        model.object.height = Qt.binding(() => height)
                    }
                }
            }
        }
    }
}
