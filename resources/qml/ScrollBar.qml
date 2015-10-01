import QtQuick 2.0

Item {
    id: scrollbar;

    property Flickable flickable : undefined

    width: 7
    anchors.right: flickable.right;
    anchors.top: flickable.top
    anchors.bottom: flickable.bottom
    z:1

    Rectangle {
        id: scrollHandle
        clip: true
        anchors.left: parent.left
        anchors.right: parent.right
        height: flickable.visibleArea.heightRatio * flickable.height
        visible: flickable.visibleArea.heightRatio < 1.0
        color: "#808080"

        Binding {
            target: scrollHandle
            property: "y"
            value: (mouseArea.drag.maximumY * flickable.contentY) / (flickable.contentHeight * (1 - flickable.visibleArea.heightRatio))
            when: !mouseArea.drag.active
        }

        Binding {
            target: flickable
            property: "contentY"
            value: ((flickable.contentHeight * (1 - flickable.visibleArea.heightRatio)) * scrollHandle.y) / mouseArea.drag.maximumY
            when: mouseArea.drag.active
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            hoverEnabled: true
            drag.target: parent
            drag.axis: Drag.YAxis
            drag.minimumY: 0
            drag.maximumY: flickable.height - scrollHandle.height
            preventStealing: true
        }
    }
}
