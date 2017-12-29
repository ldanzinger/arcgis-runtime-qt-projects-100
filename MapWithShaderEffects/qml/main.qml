
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
import QtQuick.Controls 2.2
import Esri.ArcGISRuntime 100.2

ApplicationWindow {
    id: appWindow
    width: 800
    height: 600
    title: "MapWithShaderEffects"

    ShaderEffectSource {
        id: theSource
        sourceItem: mapView
    }

    // add a mapView component
    MapView {
        id: mapView
        anchors.fill: parent

        // add a map to the mapview
        Map {
            // add the BasemapTopographic basemap to the map
            BasemapTopographic {}
        }
    }

    ShaderEffect {
        anchors.fill: parent
        visible: mode.currentText === "Wobble"
        property var source: theSource
        property real amplitude: 0.04 * wobbleSlider.value
        property real frequency: 20
        property real time: 0
        NumberAnimation on time { loops: Animation.Infinite; from: 0; to: Math.PI * 2; duration: 600 }
        fragmentShader: "qrc:///shadereffects/shaders/wobble.frag"

        Slider {
            id: wobbleSlider
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                margins: 25
            }
            value: 0.2
            width: 200
        }
    }

    ShaderEffect {
        anchors.fill: parent
        property var source: theSource
        property color tint: sliderToColor(colorizeSlider.value)
        visible: mode.currentText === "Colorize"
        fragmentShader: "qrc:///shadereffects/shaders/colorize.frag"

        Slider {
            id: colorizeSlider
            anchors {
                bottom: parent.bottom
                horizontalCenter: parent.horizontalCenter
                margins: 25
            }
            value: 0.2
            width: 200
        }
    }

    ShaderEffect {
        anchors.fill: parent
        property var source: theSource
        property real bend: 0
        property real minimize: 0
        property real side: genieSlider.value
        visible: mode.currentText === "Genie"
        SequentialAnimation on bend {
            loops: Animation.Infinite
            NumberAnimation { to: 1; duration: 700; easing.type: Easing.InOutSine }
            PauseAnimation { duration: 1600 }
            NumberAnimation { to: 0; duration: 700; easing.type: Easing.InOutSine }
            PauseAnimation { duration: 1000 }
        }
        SequentialAnimation on minimize {
            loops: Animation.Infinite
            PauseAnimation { duration: 300 }
            NumberAnimation { to: 1; duration: 700; easing.type: Easing.InOutSine }
            PauseAnimation { duration: 1000 }
            NumberAnimation { to: 0; duration: 700; easing.type: Easing.InOutSine }
            PauseAnimation { duration: 1300 }
        }
        mesh: Qt.size(10, 10)
        vertexShader: "qrc:///shadereffects/shaders/genie.vert"
        Slider {
            id: genieSlider
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 40
        }
    }

    ComboBox {
        id: mode
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        width: 150
        model: ["Default", "Wobble", "Colorize", "Genie"]
    }

    function sliderToColor(x) {
        return Qt.rgba(saturate(Math.max(2 - 6 * x, 6 * x - 4)),
                        saturate(Math.min(6 * x, 4 - 6 * x)),
                        saturate(Math.min(6 * x - 2, 6 - 6 * x)))
    }

    function saturate(x) {
        return Math.min(Math.max(x, 0), 1)
    }
}
