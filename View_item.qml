import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.1

//    Item {
    Rectangle {
        border.width: 2
        border.color: "black"
        //anchors.bottomMargin: 3
        id: slv_layout
        signal moveRequestClicked(int cur_position)
        objectName: "itemColumnLayout"
        onObjectNameChanged: {
            slvTitle.text = objectName;
        }
        /*
        onText_stringChanged: {
            console.log("onText_stringChanged");
            logText.append(text_string);
            text_string = "";
        }
*/
        width: parent.width

        property string text_string: ""
        property int button_size: 60
        property int button_margin: 2
        property int minimal_height: 200
        property var inner_model
        property int position

        height: 400

        function incomingMessage(message) {
            console.log("Incoming Message! ",message);
            //slv_log_area.append(message);
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

        Text {
            id: slvTitle
            //text: "device name"
            text: slv_layout.objectName
            font.pointSize: 12
            font.family: "Calibri"
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.topMargin: 2
            anchors.rightMargin: 2
            anchors.bottomMargin: 2
        }
        Rectangle {
            id: elementMenu
            anchors.top: slvTitle.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 2
            height: 2*button_size + 4*button_margin
            //width: parent.width
            color: "yellowgreen"
            Button {
                id: slv_butt_run;
                objectName: "slv_butt_run"
                width: button_size
                height: width
                text: "Run"
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.margins: button_margin
                enabled: false
            }
            Button {
                id: slv_butt_settings;
                objectName: "slv_butt_settings"
                width: button_size
                height: width
                text: "Settings"
                anchors.top: parent.top
                anchors.left: slv_butt_run.right
                anchors.margins: button_margin
                enabled: false
            }
            Button {
                id: slv_butt_stop;
                objectName: "slv_butt_stop"
                width: button_size
                height: width
                text: "Stop"
                anchors.top: slv_butt_run.bottom
                anchors.left: parent.left
                anchors.margins: button_margin
                enabled: false
            }
            Button {
                id: slv_butt_expand;
                objectName: "slv_butt_expand"
                width: button_size
                height: width
                text: "Move"
                anchors.top: slv_butt_settings.bottom
                anchors.left: slv_butt_stop.right
                anchors.margins: button_margin
                onClicked: {
                    console.log("clicked",objectName);
                    moveRequestClicked(position);
                }
            }
            PrefixList {
                id: prefList
                anchors.left: slv_butt_expand.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.leftMargin: 5
                anchors.topMargin: 2
                anchors.bottomMargin: 2
                anchors.rightMargin: 2
                width: 400
            }
            Text {
                id: testText
                anchors.left: prefList.right
                anchors.top: parent.top
                anchors.leftMargin: 5
                text: "pos: "+position
            }
        }//eof rectangle
 /*
        ScrollView {
            id: multiTextScroll
            anchors.top: elementMenu.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            frameVisible: true
            highlightOnFocus: true

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

            ListView {
                id: multiTextView
                anchors.fill: parent
                interactive: true
                flickableDirection: Flickable.VerticalFlick
                boundsBehavior: Flickable.StopAtBounds
                delegate: text_line
                model: id_textModel
                spacing: 2

                Component {
                    id: text_line
                    Text {
                        color: input_color
                        font.family: input_font_family
                        font.pointSize: input_font_pointsize
                        font.bold: input_bold
                        font.italic: input_italic
                        text: input_text
                    }
                }

                ListModel {
                    id :id_textModel
                    objectName: "id_textModel"
                    ListElement {
                        input_color: "blue"
                        input_font_family: "Calibri"
                        input_font_pointsize: 14
                        input_bold: false
                        input_italic: true
                        input_text: "Qt Online Installer for Linux 32-bit (23 MB) (Info)"
                    }
                    ListElement {
                        input_color: "red"
                        input_font_family: "Tahoma"
                        input_font_pointsize: 14
                        input_bold: false
                        input_italic: false
                        input_text: "Qt Online Installer for Linux 32-bit (23 MB) (Info)"
                    }
                    ListElement {
                        input_color: "green"
                        input_font_family: "Calibri"
                        input_font_pointsize: 10
                        input_bold: true
                        input_italic: false
                        input_text: "Qt Online Installer for Linux 32-bit (23 MB) (Info)"
                    }
                    ListElement {
                        input_color: "orange"
                        input_font_family: "Arial"
                        input_font_pointsize: 12
                        input_bold: false
                        input_italic: false
                        input_text: "Qt Online Installer for Linux 32-bit (23 MB) (Info)"
                    }
                }

                Component.onCompleted: {
                    //landingTarget = pl_listView;
                    multiTextScroll.contentItem = multiTextView;
                    //completed = 1;
                }
            }//eof listview


        }//eof scrollview
*/
/*
        Flickable {
            id: flick
            anchors.top: elementMenu.bottom
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 2
            visibleArea.yPosition: contentHeight
            //onMovementEnded: {
            //    visibleArea.yPosition = contentHeight;
            //}

            /*
            states: State {
                name: "autoscroll"
                PropertyChanges {
                    target: flick
                    contentY: logText.height - height
                }
            }
            onMovementEnded: {
                if (contentY === logText.height - height) {
                    state = "autoscroll"
                }
                else {
                    state = ""  // default state
                }
            }
            */
    TextArea {
        id: logText
        //anchors.fill: parent
        anchors.top: elementMenu.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 2
        textFormat: TextEdit.RichText
        readOnly: true
        text: text_string

        onTextChanged: {

        }

        Component.onCompleted: {
            prefList.prefix_model = inner_model;


            //inner_model = prefList.prefix_model;
            //console.log("set inner model to ",inner_model);

            //logText.append("<b>fgsdgfd</b> sdfsdf <i>regrge</i>");
            //logText.append("<font size=14 color=red>sadgasgewrg</font>");
        }
    }

        //}

}
