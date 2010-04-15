import Qt 4.6

Rectangle {
    width: 300; height: 400; color: "white"

    ListModel {
        id: appModel
        ListElement { name: "Music"; icon: "pics/AudioPlayer_48.png" }
        ListElement { name: "Movies"; icon: "pics/VideoPlayer_48.png" }
        ListElement { name: "Camera"; icon: "pics/Camera_48.png" }
        ListElement { name: "Calendar"; icon: "pics/DateBook_48.png" }
        ListElement { name: "Messaging"; icon: "pics/EMail_48.png" }
        ListElement { name: "Todo List"; icon: "pics/TodoList_48.png" }
        ListElement { name: "Contacts"; icon: "pics/AddressBook_48.png" }
    }

    Component {
        id: appDelegate
        Item {
            id: item
            width: 100; height: 100
            Image { id: myIcon; y: 20; anchors.horizontalCenter: parent.horizontalCenter; source: icon }
            Text { anchors.top: myIcon.bottom; anchors.horizontalCenter: parent.horizontalCenter; text: name }
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    gridView.currentIndex = index;
                    eventHandler.itemClicked(name);
                }
            }
            Keys.onReturnPressed: {
                eventHandler.itemClicked(name);
            }
        }
    }

    Component {
        id: appHighlight
        Rectangle { width: 80; height: 80; color: "lightsteelblue" }
    }

    GridView {
        id: gridView
        anchors.fill: parent
        cellWidth: 100; cellHeight: 100
        model: appModel; delegate: appDelegate
        highlight: appHighlight
        focus: true
    }
}
