import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.2

ScrollView {
    id: id_scrollView
    anchors.fill: parent
    anchors.bottomMargin: 3
    objectName: "ScrollView"
    frameVisible: true
    highlightOnFocus: true
    signal processMoveRequest(int view_pos, int tab_pos)
    property int tab_position
    property int log_window_capacity
    property var landingTarget
    property int completed: 0
    property var modelList: []
    property string tmps
    function addObject(input_name) {
        console.log("get object name: ",input_name);

        //ListModel new_component;

        //var new_component = Qt.createComponent(ListModel);

        var new_object = dynamicModel.createObject(id_scrollView);
        //var new_object = dynamicModel.createObject();
 /*
        console.log("new object is ",new_object);
        new_object.append({"icolor":"red", "ichecked":true, "ipos":new_object.count});
        new_object.append({"icolor":"green", "ichecked":false, "ipos":new_object.count});
        new_object.append({"icolor":"brown", "ichecked":true, "ipos":new_object.count});
        new_object.append({"icolor":"yellow", "ichecked":false, "ipos":new_object.count});
        new_object.append({"icolor":"black", "ichecked":false, "ipos":new_object.count});
*/
        id_listModel.append({"name":input_name, "in_text":"<b>Hello world</b>",
                            "in_model": new_object, "in_pos": id_listModel.count } );
    }
    function addExistingObject(ex_object) {
        //console.log("Model name: ",ex_object.name, " text: ",ex_object.in_text);
        id_listModel.append(ex_object);
        console.log("Model ",id_listModel," now contains ",id_listModel.count," elements");
        id_listModel.get(id_listModel.count-1).in_pos = id_listModel.count-1;
        //console.log("fsdf= ",id_listModel.get(id_listModel.count-1).in_pos);
    }
    function deleteObjectFromModel(pos) {
        id_listModel.remove(pos,1);
        updateModelPositions();
    }
    function updateModelPositions() {
        for (var idx=0; idx<id_listModel.count; idx++) {
            id_listModel.get(idx).in_pos = idx;
        }
    }

    function getItemFromModel(pos) {
        return id_listModel.get(pos);
    }
    function getItemNameFromPos(pos) {
        return id_listModel.get(pos).name;
    }
    function getItemsCount() {
        return id_listModel.count;
    }

    function selectNewColor(index) {
        var colors_number = 9;
        var fixed_index = index;
        var ret_color = "";
        while (fixed_index >= colors_number) {
            fixed_index -= colors_number;
        }
        switch (fixed_index) {
            case 0:
                ret_color = "black";
            break;
            case 1:
                ret_color = "midnightblue";
            break;
            case 2:
                ret_color = "firebrick";
            break;
            case 3:
                ret_color = "green";
            break;
            case 4:
                ret_color = "saddlebrown";
            break;
            case 5:
                ret_color = "gray";
            break;
            case 6:
                ret_color = "goldenrod";
            break;
            case 7:
                ret_color = "seagreen";
            break;
            case 8:
                ret_color = "slateblue";
            break;
        }

        return ret_color;
    }

    function addMessageToLog(pos,prefix,time,msg) {
        //console.log("pos ",pos," message ",msg);
        var cur_model = id_listModel.get(pos);
        var p_model = cur_model.in_model;
        var prefix_found = false;
        var idx;
        //find prefix or create new
        for (idx=0; idx<p_model.count; idx++) {
            if (p_model.get(idx).iname === prefix) {
                prefix_found = true;
                break;
            }
        }
        if (prefix_found === false) {
            //create new prefix (TODO: add colors default rotation)
            idx = p_model.count;
             p_model.append({"icolor":selectNewColor(idx), "ichecked":true, "ipos":p_model.count, "iname":prefix});
            //idx = p_model.count - 1;
        }
        var used_prefix = p_model.get(idx);

        if (used_prefix.ichecked) {
            var new_string = "<br><font color="+used_prefix.icolor+">["+prefix+"] "+time+": "+msg+"</font>";
            cur_model.in_text += new_string;
            //cur_model.in_text = new_string;

            //check for maximum field size
            //console.log("text len = ",cur_model.in_text.length);
            if (cur_model.in_text.length > log_window_capacity) {
                cur_model.in_text = cur_model.in_text.substr(100, cur_model.in_text.length-1);
                //tmps = cur_model.in_text.co;
                //cur_model.in_text = tmps.remove(0,100);
            }
        }
    }
    function accessToInnerModel(pos) {
        var cur_model = id_listModel.get(pos);
        var p_model = cur_model.in_model;
        console.log(" Inner model= ",p_model);
        p_model.get(0).icolor = "green";
    }


    style: ScrollViewStyle {
            scrollToClickedPosition: true
            //transientScrollBars: true
            handle: Item {
                implicitWidth: 16
                implicitHeight: 26
                Rectangle {
                    color: "#424246"
                    anchors.fill: parent
                    anchors.topMargin: 6
                    anchors.leftMargin: 0
                    anchors.rightMargin: 2
                    anchors.bottomMargin: 6
                }
            }
            scrollBarBackground: Item {
                implicitWidth: 20
                implicitHeight: 26
            }
        }

    ListView {

        id: id_listView
        //anchors.fill: parent
        objectName: "ListView"
        spacing: 2
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 2
        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        delegate: view_component
        model: id_listModel

        Component {
            id: dynamicModel
            ListModel {
            }
        }

        ListModel {
            id :id_listModel
            objectName: "ListModel"
        }

        //delegate: View_item2.Item
        Component {
            id: view_component
            View_item {
                objectName: name
                text_string: in_text
                inner_model: in_model
                position: in_pos
                onMoveRequestClicked: {
                    processMoveRequest(cur_position,tab_position);
                }
            }
        }

        Component.onCompleted: {
            landingTarget = id_listView;
            id_scrollView.contentItem = id_listView;
            console.log("Flickable item is ",id_scrollView.flickableItem.objectName);
            completed = 1;
        }

    }
}
