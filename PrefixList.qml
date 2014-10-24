import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

Rectangle {
    id: pl_base
    color: "white"
        property var prefix_model
//anchors.top: parent.top
//anchors.bottom: parent.bottom
//width: 400
    ScrollView {
        id: pl_mainView
        objectName: "pl_mainView"
        anchors.fill: parent
        frameVisible: true
        highlightOnFocus: true
        property var landingTarget: pl_listView
        //property var prefix_model
        property int completed: 0

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
            id: pl_listView
            anchors.fill: parent
            interactive: true
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            delegate: view_component
            //model: id_listModel
            model: prefix_model
            spacing: 2

            Component {
                id: view_component
                PrefixElement {
                    textColor: icolor
                    isChecked: ichecked
                    position: ipos
                    prefix_name: iname
                    onCheckChanged: {
                        prefix_model.get(cur_position).ichecked = new_state;
                    }
                }
            }

            Component.onCompleted: {
                //landingTarget = pl_listView;
                pl_mainView.contentItem = pl_listView;
                //prefix_model = id_listModel;
                //console.log("set prefix_model to ",prefix_model);
                completed = 1;
            }
        }
    }

}
