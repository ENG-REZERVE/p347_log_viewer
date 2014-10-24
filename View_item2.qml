import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0

/*
Rectangle {
    id: logViewItem
    objectName: "test2_item"
    //anchors.top: parent.top
    //anchors.fill: parent
    Layout.fillWidth: true
*/
//Rectangle {
    Item {
        //clip: true
        id: slv_layout
        signal mouseClicked();
        objectName: "itemColumnLayout"
        onObjectNameChanged: {
            slvTitle.text = objectName;
        }
        width: parent.width

        //for flow parent, disable
       // anchors.left: parent.left
        //anchors.right: parent.right

        //anchors.fill: parent
        //anchors.margins: 1

        property int minimal_height: 200
        height: 400
        //color: "green"

        function incomingMessage(message) {
            slv_log_area.append(message);
        }
        MouseArea {
            property bool is_pressed: false
            property int initial_y: 0
            property int proposed_y: 0
            id: resizeStick
            enabled: true
            anchors.bottom: parent.bottom
            height: 10
            width: parent.width
            hoverEnabled: true
            //acceptedButtons: Qt.NoButton
            //cursorShape: Qt.SizeVerCursor
            onEntered: {
                cursorShape = Qt.SizeVerCursor;
            }
            onPressed: {
                is_pressed = true;
                initial_y = mouseY;
                //console.log("onPressed, Y=",mouseY);
            }
            onReleased: {
                is_pressed = false;
                //console.log("onPressed, Y=",mouseY);
            }
            onMouseYChanged: {
                if (is_pressed) {
                    proposed_y = slv_layout.height + mouseY - initial_y;
                    if (proposed_y >= slv_layout.minimal_height) {
                        slv_layout.height += (mouseY - initial_y);
                        //console.log("new height =",slv_layout.height);
                        initial_y = mouseY;
                    }
                }
            }
        }

        //Layout.fillWidth: true

        Text {
            id: slvTitle
            text: "device name"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.margins: 2
        }
        Rectangle {
            anchors.top: slvTitle.bottom
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.topMargin: 2
            //width: parent.width
            color: "blue"
            Button {
                id: slv_butt_run;
                objectName: "slv_butt_run"
                width: 60
                height: width
                text: "Run"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: 2
            }
            Button {
                id: slv_butt_settings;
                objectName: "slv_butt_settings"
                width: 60
                height: width
                text: "Settings"
                anchors.top: parent.top
                anchors.left: slv_butt_run.right
                anchors.margins: 2
            }
            Button {
                id: slv_butt_stop;
                objectName: "slv_butt_stop"
                width: 60
                height: width
                text: "Stop"
                anchors.top: slv_butt_run.bottom
                anchors.left: parent.left
                anchors.margins: 2
            }
            Button {
                id: slv_butt_expand;
                objectName: "slv_butt_expand"
                width: 60
                height: width
                text: "Expand"
                anchors.top: slv_butt_settings.bottom
                anchors.left: slv_butt_stop.right
                anchors.margins: 2
                onClicked: {
                    console.log("clicked",objectName);
                    mouseClicked();
                }
            }
/*
            GridLayout {
                anchors.margins: 2
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                id: buttons_grid
                rows: 2
                columns: 2
                rowSpacing: 2
                columnSpacing: 2
                flow: GridLayout.TopToBottom
                //width: 140

            }
*/
            TextArea {
                id: slv_log_area
                anchors.left: slv_butt_expand.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 3
            }
        }
    }

