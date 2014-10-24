import QtQuick 2.1

Rectangle {
    id: colorPickMain
    width: 100
    height: 100
    ListModel {
        id: pickModel
        ListElement {
            bgcolor: "red"
        }
        ListElement {
            bgcolor: "green"
        }
        ListElement {
            bgcolor: "blue"
        }
        ListElement {
            bgcolor: "black"
        }
        ListElement {
            bgcolor: "brown"
        }
        ListElement {
            bgcolor: "gray"
        }
    }

    GridView {
        anchors.fill: parent
        model: pickModel
        delegate: Rectangle {
            width: 30
            height: 30
            color: bgcolor
        }
    }
}
