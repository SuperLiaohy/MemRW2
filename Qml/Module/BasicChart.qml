import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FluentUI

FluFrame {

    ListView {  // 推荐用 ListView 或 GridView，性能更好
        anchors.fill: parent
        model: lineModel
        interactive: false

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
