import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Rectangle {
    id: dialogRect
    color: Qt.rgba(0.0, 0.0, 0.0, 0.5)
    anchors.fill: parent
    property var tab_link
    property int src_view_pos
    property int src_tab_pos
    signal requestNewTab()

    function createModel(from_pos, view_idx) {
        src_view_pos = view_idx;
        src_tab_pos = from_pos;
        for (var idx=0; idx<tab_link.count; idx++) {
            if (idx !== from_pos)
                targetChoiceModel.append({"in_pos": idx});
        }
    }

    MouseArea { // for blocking all underlayers
        anchors.fill: parent
        //acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        acceptedButtons: "AllButtons"
    }

    Rectangle {
        id: mainRect
        color: "lightgray"
        width: 200
        height: 300
        anchors.centerIn: parent

        ListModel {
            id: targetChoiceModel
        }

        Component {
            id: choiceDelegate
            Button {
                id: deleButt
                /*
                style: ButtonStyle {
                    background: Rectangle {
                        implicitHeight: 30
                        border.color: "black"
                        radius: 4
                        color: "silver"
                    }
                }
                */
                implicitHeight: 30
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 5
                text: "To tab "+position
                property int position: in_pos
                onClicked: {
                    var tab_src = tabHolder.getTab(src_tab_pos).item;
                    var tab_dst = tabHolder.getTab(position).item;
                    tab_dst.addExistingObject(tab_src.getItemFromModel(src_view_pos));
                    tab_src.deleteObjectFromModel(src_view_pos);

                    dialogRect.destroy();
                }
            }
        }

        Text {
            id: helpText
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5
            text: "Select where to move view: "
        }

        ScrollView {
            id: choiceScroll
            anchors.margins: 5
            anchors.top: helpText.bottom
            anchors.bottom: buttNewTab.top
            anchors.left: parent.left
            anchors.right: parent.right

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
                id: choiceList
                model: targetChoiceModel
                delegate: choiceDelegate
                anchors.fill: parent
                spacing: 5
            }
        }

        Button {
            id: buttNewTab
            anchors.bottom: buttCancel.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5
            height: 30
            text: "To new Tab"
            onClicked: {
                requestNewTab();

                var tab_src = tabHolder.getTab(src_tab_pos).item;
                var tab_dst = tabHolder.getTab(tab_link.count-1).item;
                tab_dst.addExistingObject(tab_src.getItemFromModel(src_view_pos));
                tab_src.deleteObjectFromModel(src_view_pos);

                dialogRect.destroy();
            }
        }

        Button {
            id: buttCancel
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 5
            height: 30
            text: "Cancel"
            onClicked: {
                dialogRect.destroy();
            }
        }
    }

}
