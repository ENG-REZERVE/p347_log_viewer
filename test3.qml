import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

ApplicationWindow {
    visible: true
    title: "p347 Log Viewer"
    //visibility: "FullScreen"
    width: 800
    height: 600
    id: appWindow
    objectName: "appWindow"
    //minimumWidth: mainLayout.Layout.minimumWidth + 2 * margin
    //minimumHeight: mainLayout.Layout.minimumHeight + 2 * margin
    property int margin: 7

    //property var viewsList: []
    property var current_tab
    property var tab_src
    property var tab_dst
    //property int log_window_capacity

    function onProcessMove(view_pos,tab_pos) {
        console.log("Main file move pos ",tab_pos,", view ",view_pos);

        var new_component = Qt.createComponent("MoveViewDialog.qml");
        var new_object = new_component.createObject(appWindow);
        new_object.tab_link = tabHolder;
        new_object.createModel(tab_pos,view_pos);
        new_object.requestNewTab.connect(addNewTab);
        //var new_model = dynamicModel.createObject();
    }

    function addNewTab() {
        tabHolder.addTab(tabHolder.count,Qt.createComponent("MainTab.qml"));
        tabHolder.currentIndex = tabHolder.count - 1;
        current_tab = tabHolder.getTab(tabHolder.count-1).item;
        current_tab.log_window_capacity = log_window_capacity;
        console.log("set window capacity",log_window_capacity);
        current_tab.tab_position = tabHolder.count-1;
        current_tab.processMoveRequest.connect(appWindow.onProcessMove);
    }

    /*
    function addLogView(v_name,tab,target) {
        /*
        new_component = Qt.createComponent("View_item2.qml");
        new_object = new_component.createObject(target);
        tab.addObject(v_name);
//        target.delegate = new_object.id;
        //target.add(new_object);
        //new_object.objectName = v_name;
        viewsList.push(new_object);
    }
*/

/*
   Button {
       id: addTabButton
       text: "addTab"
       anchors.top: parent.top
       anchors.left: parent.left
       function onCompleted() {
            console.log("OnCompleted");
       }

       onClicked: {
           console.log("onClicked addTab");
            addNewTab();
           //current_tab.addObject("new_dev");
           //current_tab.processMoveRequest.connect(appWindow.onProcessMove);
       }
   }
   Button {
       id: moveComponentButton
       anchors.top: parent.top
       anchors.left: addTabButton.right
       anchors.leftMargin: 5
       text: "move"
       onClicked: {

           tab_src = tabHolder.getTab(0).item;
           tab_dst = tabHolder.getTab(1).item;

           tab_dst.addExistingObject(tab_src.getItemFromModel(0));
        }
   }
   Button {
       id: accInnerModel
       anchors.top: parent.top
       anchors.left: moveComponentButton.right
       anchors.leftMargin: 5
       text: "Modify color"
       onClicked: {
           tabHolder.getTab(0).item.accessToInnerModel(0);
       }
   }
*/
   Button {
       id: buttAddText
       anchors.top: parent.top
       anchors.left: parent.left
       anchors.leftMargin: 5
       text: "AddText"
       onClicked: {
           cppLogView.acceptCommand("Hulitam");
       }
   }

   TabView {
        id: tabHolder
        anchors.top: buttAddText.bottom
        //anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        signal fromTab()
        //anchors.fill: parent
        Component.onCompleted: {
            //addNewTab();
            /*
            addTab("Main Tab",Qt.createComponent("MainTab.qml"));
            current_tab = tabHolder.getTab(0).item;
            current_tab.tab_position = 0;
            */
            //current_tab.addObject("Local Device");
            //current_tab.addObject("dev2");
            //current_tab.processMoveRequest.connect(appWindow.onProcessMove);
        }

        Connections {
            target: cppLogView
            property int idx
            property int idx2
            property int icount
            //property bool found
            property var vtab
            property var dev_name
            onAddLogMessage: {
                var dev_found = false;
                //1) Check for device name, have we it already?
                for (idx=0; idx<tabHolder.count; idx ++) {
                    vtab = tabHolder.getTab(idx).item;
                    //TODO: check for NULL ?
                    icount = vtab.getItemsCount();
                    for (idx2=0; idx2<icount; idx2++) {
                        dev_name = vtab.getItemNameFromPos(idx2);
                        //console.log("item name =",dev_name,"incoming name =",d_name);
                        if (dev_name === d_name) {
                            //console.log("Found view mathing device ",d_name);
                            dev_found = true;
                            break;
                        }
                    }
                    if (dev_found === true)
                        break;
                }
                if (dev_found === false) {
                    if (tabHolder.count === 0) {//create at least one tab
                        addNewTab();
                    }
                    //TODO: create and use settings 'where to create new view'
                    vtab = tabHolder.getTab(tabHolder.currentIndex).item;
                    vtab.addObject(d_name);
                    idx2 = vtab.getItemsCount()-1;
                    console.log("Created new view for device ",d_name," at tab ",tabHolder.currentIndex," at pos ",idx2);
                }
                //now we have proper vtab+idx2 for message adding
                vtab.addMessageToLog(idx2,m_prefix,m_time,m_message);
            }
        }

   }


}//eof app window
