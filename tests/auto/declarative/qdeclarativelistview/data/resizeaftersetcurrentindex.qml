import QtQuick 1.1

Rectangle {
    id: root
    anchors.fill: parent

    ListView {
        id: listView
        objectName: "listView"
        clip: true

        anchors.fill: parent

        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        preferredHighlightBegin: 0
        preferredHighlightEnd: width
        highlightRangeMode: ListView.StrictlyEnforceRange

        highlightMoveDuration: 1
        highlightResizeDuration: 1

        cacheBuffer: 2 * width

        model: superModel
        focus: true

        delegate: Rectangle {
            id: delegateItem
            width: ListView.view.width
            height: ListView.view.height

            color: dcolor

            Text {
                anchors.centerIn: parent
                text: textStr
                font.pixelSize: 52
            }
        }

    }

    ListModel {
        id: superModel
        objectName: "listModel"

        ListElement { textStr: "0"; dcolor: "green" }
        ListElement { textStr: "1"; dcolor: "red" }
        ListElement { textStr: "2"; dcolor: "yellow" }
        ListElement { textStr: "3"; dcolor: "blue" }
    }
}

