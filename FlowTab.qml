import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Flow {
        id: mainFlow
        objectName: "mainFlow"
        //flow: Flow.TopToBottom
        spacing: 2
        anchors.fill: parent

        move: Transition {
            NumberAnimation { properties: "y"; easing.type: Easing.OutBounce }
        }
/*
        MouseArea {
            anchors.fill: parent
            onWheel: {
                mainFlow.s
            }
        }
*/
}

/*
ScrollView {
    id: id_scrollView
    anchors.fill: parent
    objectName: "ScrollView"
    frameVisible: true
    highlightOnFocus: true
    property var landingTarget

    style: ScrollViewStyle {
            transientScrollBars: true
            handle: Item {
                implicitWidth: 14
                implicitHeight: 26
                Rectangle {
                    color: "#424246"
                    anchors.fill: parent
                    anchors.topMargin: 6
                    anchors.leftMargin: 4
                    anchors.rightMargin: 4
                    anchors.bottomMargin: 6
                }
            }
            scrollBarBackground: Item {
                implicitWidth: 14
                implicitHeight: 26
            }
        }


    Flow {
        id: mainFlow
        objectName: "mainFlow"
        flow: Flow.TopToBottom
        anchors.fill: parent
        //effectiveLayoutDirection: Flow.TopToBottom
        spacing: 2

        Component.onCompleted: {
            landingTarget = mainFlow;
            id_scrollView.contentItem = mainFlow;
            console.log("Flickable item is ",id_scrollView.flickableItem.objectName);
        }
    }
}
*/
