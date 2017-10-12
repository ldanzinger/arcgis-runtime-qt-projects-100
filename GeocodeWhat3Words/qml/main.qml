
// Copyright 2016 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.6
import QtQuick.Controls 1.4
import Esri.GeocodeWhat3Words 1.0

GeocodeWhat3Words {
    width: 800
    height: 600

    // Create MapQuickView here, and create its Map etc. in C++ code
    MapView {
        anchors.fill: parent
        objectName: "mapView"
        // set focus to enable keyboard navigation
        focus: true
    }

    Rectangle {
        anchors {
            right: parent.right
            top: parent.top
            margins: 10
        }
        height: 60
        width: 225
        radius: 3
        color: "lightgray"
        border {
            width: 1
            color: "darkgray"
        }

        Column {
            anchors {
                fill: parent
                margins: 5
            }

            spacing: 10

            Text {
                text: "What3Words:"
                font.pointSize: 18
            }

            Text {
                text: what3words
            }
        }
    }
}
