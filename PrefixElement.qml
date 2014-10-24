import QtQuick 2.1
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.0

Rectangle {
    id: prefixElement
    anchors.margins: 2
    height: pel_checkbox.height + anchors.margins
    //width: parent.width
    color: "lightgrey"
    anchors.left: parent.left
    anchors.right: parent.right
    signal checkChanged(bool new_state, int cur_position)
    property bool isChecked
    property int position
    property color textColor: "black"
    property string prefix_name
    CheckBox {
        id: pel_checkbox
        style: CheckBoxStyle {
                indicator: Rectangle {
                        implicitWidth: 30
                        implicitHeight: 30
                        radius: 3
                        border.color: control.activeFocus ? "darkblue" : "gray"
                        border.width: 1
                        Rectangle {
                            visible: control.checked
                            color: "#555"
                            border.color: "#333"
                            radius: 1
                            anchors.margins: 4
                            anchors.fill: parent
                        }
                }
            }
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 3
        onClicked: {
            console.log("New state of pos ",position," is ", checked);
            checkChanged(checked, position);
        }
        height: 30
        width: height
        checked: isChecked
    }
    Text {
        id: pel_prefixname
        anchors.left: pel_checkbox.right
        anchors.leftMargin: 3
        anchors.top: parent.top
        text: prefix_name
        font.family: "Calibri"
        font.pointSize: 12
        color: textColor
    }
    Rectangle {
        id: pel_colorpicker
        anchors.right: parent.right
        anchors.top: parent.top
        height: pel_checkbox.height
        width: height
        color: textColor
        //TODO: color picker call
    }
}
