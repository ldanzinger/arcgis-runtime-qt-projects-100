
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

import QtQuick 2.3
import QtQuick.Controls 1.2
import QtGraphicalEffects 1.0
import QuickPromise 1.0
import Esri.ArcGISRuntime 100.0
import Esri.ArcGISExtras 1.1

ApplicationWindow {
    id: appWindow
    width: 800
    height: 600
    title: "Quick Promise"

    property real scaleFactor: System.displayScaleFactor
    property Envelope tileCacheExtent: null
    property url outputTileCachePath: System.temporaryFolder.url + "/TileCacheQml_%1.tpk".arg(new Date().getTime().toString())
    property string statusText: ""
    property string tiledServiceUrl: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/World_Street_Map/MapServer"
    property string dataPath: System.userHomePath + "/ArcGIS/Runtime/Data/"
    property url outputGdb: System.temporaryFolder.url + "/WildfireQml_%1.geodatabase".arg(new Date().getTime().toString())
    property string featureServiceUrl: "http://sampleserver6.arcgisonline.com/arcgis/rest/services/Sync/WildfireSync/FeatureServer"
    property Envelope generateExtent: null
    property var generateLayerOptions: []
    property TileCache outputTileCache
    property Geodatabase outputGeodatabase
    property ExportTileCacheParameters params

    // Create MapView that contains a Map
    MapView {
        id: mapView
        anchors.fill: parent
        Map {
            id: map
            // Nest an ArcGISTiledLayer in the Basemap
            Basemap {
                ArcGISTiledLayer {
                    id: tiledLayer
                    url: tiledServiceUrl
                }
            }

            onLoadStatusChanged: {
                if (loadStatus === Enums.LoadStatusLoaded) {
                    // add the feature layers
                    geodatabaseSyncTask.load();
                }
            }

            // set an initial viewpoint
            ViewpointExtent {
                Envelope {
                    xMax: 12362601.050868368
                    xMin: 10187678.26582548
                    yMax: 2567213.6854449743
                    yMin: 936021.5966628084
                    spatialReference: SpatialReference.createWebMercator()
                }
            }
        }
    }

    // Create promise
    Promise {
        resolveWhen: Q.all([exportTask.exportJobDone, geodatabaseSyncTask.generateJobDone])

        // once all promises are fulfilled...
        onFulfilled: {
            // update the status text
            statusText = "Complete!"
            // display the output tile cache
            exportTask.displayOutputTileCache(outputTileCache);
            // display the gdb data from the sync task
            geodatabaseSyncTask.displayLayersFromGeodatabase(outputGeodatabase);
            // hide the busy window
            busyWindow.hideWindow(3000);
        }
    }

    // create the GeodatabaseSyncTask to generate the local geodatabase
    GeodatabaseSyncTask {
        id: geodatabaseSyncTask
        url: featureServiceUrl

        signal generateJobDone()

        onErrorChanged: {
            console.log("ERROR:", error.message)
        }

        onLoadStatusChanged: {
            if (loadStatus === Enums.LoadStatusLoaded) {
                var idInfos = featureServiceInfo.layerInfos;
                for (var i = 0; i < idInfos.length; i++) {
                    // add the layer to the map
                    var featureLayerUrl = featureServiceInfo.url + "/" + idInfos[i].infoId;
                    var serviceFeatureTable = ArcGISRuntimeEnvironment.createObject("ServiceFeatureTable", {url: featureLayerUrl});
                    var featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer", {featureTable: serviceFeatureTable});
                    map.operationalLayers.append(featureLayer);

                    // add a new GenerateLayerOption to array for use in the GenerateGeodatabaseParameters
                    var layerOption = ArcGISRuntimeEnvironment.createObject("GenerateLayerOption", {layerId: idInfos[i].infoId});
                    generateLayerOptions.push(layerOption);
                    generateParameters.layerOptions = generateLayerOptions;
                }
            }
        }

        function executeGenerate() {
            // execute the asynchronous task and obtain the job
            var generateJob = generateGeodatabase(generateParameters, outputGdb);

            // check if the job is valid
            if (generateJob) {

                // show the generate window
                busyWindow.visible = true;

                // connect to the job's status changed signal to know once it is done
                generateJob.jobStatusChanged.connect(function() {
                    switch(generateJob.jobStatus) {
                    case Enums.JobStatusFailed:
                        statusText = "Generate failed";
                        busyWindow.hideWindow(5000);
                        break;
                    case Enums.JobStatusNotStarted:
                        statusText = "Job not started";
                        break;
                    case Enums.JobStatusPaused:
                        statusText = "Job paused";
                        break;
                    case Enums.JobStatusStarted:
                        statusText = "In progress...";
                        break;
                    case Enums.JobStatusSucceeded:
                        outputGeodatabase = generateJob.geodatabase;
                        generateJobDone(); // emit that the job is done
                        break;
                    default:
                        break;
                    }
                });

                // start the job
                generateJob.start();
            } else {
                // a valid job was not obtained, so show an error
                busyWindow.visible = true;
                statusText = "Generate failed";
                busyWindow.hideWindow(5000);
            }
        }

        function displayLayersFromGeodatabase(geodatabase) {
            // remove the original online feature layers
            map.operationalLayers.clear();

            // load the geodatabase to access the feature tables
            geodatabase.loadStatusChanged.connect(function() {
                if (geodatabase.loadStatus === Enums.LoadStatusLoaded) {
                    // create a feature layer from each feature table, and add to the map
                    for (var i = 0; i < geodatabase.geodatabaseFeatureTables.length; i++) {
                        var featureTable = geodatabase.geodatabaseFeatureTables[i];
                        var featureLayer = ArcGISRuntimeEnvironment.createObject("FeatureLayer");
                        featureLayer.featureTable = featureTable;
                        map.operationalLayers.append(featureLayer);
                    }

                    // unregister geodatabase since there will be no edits uploaded
                    geodatabaseSyncTask.unregisterGeodatabase(geodatabase);

                    // hide the extent rectangle and download button
                    extentRectangle.visible = false;
                    downloadButton.visible = false;
                }
            });
            geodatabase.load();
        }
    }

    // create the generate geodatabase parameters
    GenerateGeodatabaseParameters {
        id: generateParameters
        extent: generateExtent
        outSpatialReference: SpatialReference.createWebMercator()
        returnAttachments: false
    }

    // Create ExportTileCacheTask
    ExportTileCacheTask {
        id: exportTask
        url: tiledServiceUrl
        property var exportJob

        signal exportJobDone()

        onCreateDefaultExportTileCacheParametersStatusChanged: {
            if (createDefaultExportTileCacheParametersStatus === Enums.TaskStatusCompleted) {
                params = defaultExportTileCacheParameters;

                // export the cache with the parameters
                executeExportTileCacheTask(params);
            }
        }

        onErrorChanged: {
            console.log("ERROR:", error.message)
        }

        function generateDefaultParameters() {
             // generate the default parameters with the extent and map scales specified
             exportTask.createDefaultExportTileCacheParameters(tileCacheExtent, mapView.mapScale, tiledLayer.maxScale);
        }


        function executeExportTileCacheTask(params) {
            // execute the asynchronous task and obtain the job
            exportJob = exportTask.exportTileCache(params, outputTileCachePath);

            // check if job is valid
            if (exportJob) {
                // show the export window
                busyWindow.visible = true;

                // connect to the job's status changed signal to know once it is done
                exportJob.jobStatusChanged.connect(function() {
                    switch(exportJob.jobStatus) {
                    case Enums.JobStatusFailed:
                        statusText = "Export failed";
                        busyWindow.hideWindow(5000);
                        break;
                    case Enums.JobStatusNotStarted:
                        statusText = "Job not started";
                        break;
                    case Enums.JobStatusPaused:
                        statusText = "Job paused";
                        break;
                    case Enums.JobStatusStarted:
                        statusText = "In progress...";
                        break;
                    case Enums.JobStatusSucceeded:
                        outputTileCache = exportJob.result;
                        exportJobDone(); // emit that the export is done
                        break;
                    default:
                        break;
                    }
                });

                exportJob.start();
            } else {
                busyWindow.visible = true;
                statusText = "Export failed";
                busyWindow.hideWindow(5000);
            }
        }

        function displayOutputTileCache(tileCache) {
            // create a new tiled layer from the output tile cache
            var tiledLayer = ArcGISRuntimeEnvironment.createObject("ArcGISTiledLayer", { tileCache: tileCache } );

            // create a new basemap with the tiled layer
            var basemap = ArcGISRuntimeEnvironment.createObject("Basemap");
            basemap.baseLayers.append(tiledLayer);

            // set the new basemap on the map
            map.basemap = basemap;

            // zoom to the new layer and hide window once loaded
            tiledLayer.loadStatusChanged.connect(function() {
                if (tiledLayer.loadStatus === Enums.LoadStatusLoaded) {
                    extentRectangle.visible = false;
                    downloadButton.visible = false;
                    mapView.setViewpointScale(mapView.mapScale * .5);
                }
            });
        }
    }

    Rectangle {
        id: extentRectangle
        anchors.centerIn: parent
        width: parent.width - (50 * scaleFactor)
        height: parent.height - (125 * scaleFactor)
        color: "transparent"
        border {
            color: "red"
            width: 3 * scaleFactor
        }
    }

    // Create the download button to take data offline
    Rectangle {
        id: downloadButton
        property bool pressed: false
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 25 * scaleFactor
        }

        width: 130 * scaleFactor
        height: 35 * scaleFactor
        color: pressed ? "#959595" : "#D6D6D6"
        radius: 5
        border {
            color: "#585858"
            width: 1 * scaleFactor
        }

        Row {
            anchors.fill: parent
            spacing: 5
            Image {
                width: 38 * scaleFactor
                height: width
                source: "qrc:/Resources/download.png"
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "Go Offline"
                font.pixelSize: 14 * scaleFactor
                color: "#474747"
            }
        }

        MouseArea {
            anchors.fill: parent
            onPressed: downloadButton.pressed = true
            onReleased: downloadButton.pressed = false
            onClicked: {
                getRectangleEnvelope();
            }

            function getRectangleEnvelope() {
                var corner1 = mapView.screenToLocation(extentRectangle.x, extentRectangle.y);
                var corner2 = mapView.screenToLocation((extentRectangle.x + extentRectangle.width), (extentRectangle.y + extentRectangle.height));
                var envBuilder = ArcGISRuntimeEnvironment.createObject("EnvelopeBuilder");
                envBuilder.setCorners(corner1, corner2);
                tileCacheExtent = GeometryEngine.project(envBuilder.geometry, SpatialReference.createWebMercator());
                generateExtent = GeometryEngine.project(envBuilder.geometry, SpatialReference.createWebMercator());
                exportTask.generateDefaultParameters();
                geodatabaseSyncTask.executeGenerate();
            }
        }
    }

    // Create a window to display the busy window
    Rectangle {
        id: busyWindow
        anchors.fill: parent
        color: "transparent"
        visible: false
        clip: true

        RadialGradient {
            anchors.fill: parent
            opacity: 0.7
            gradient: Gradient {
                GradientStop { position: 0.0; color: "lightgrey" }
                GradientStop { position: 0.5; color: "black" }
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: mouse.accepted = true
            onWheel: wheel.accepted = true
        }

        Rectangle {
            anchors.centerIn: parent
            width: 125 * scaleFactor
            height: 100 * scaleFactor
            color: "lightgrey"
            opacity: 0.8
            radius: 5
            border {
                color: "#4D4D4D"
                width: 1 * scaleFactor
            }

            Column {
                anchors {
                    fill: parent
                    margins: 10 * scaleFactor
                }
                spacing: 10

                BusyIndicator {
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: statusText
                    font.pixelSize: 16 * scaleFactor
                }
            }
        }

        Timer {
            id: hideWindowTimer

            onTriggered: busyWindow.visible = false;
        }

        function hideWindow(time) {
            hideWindowTimer.interval = time;
            hideWindowTimer.restart();
        }
    }

    FileFolder {
        path: dataPath

        // create the data path if it does not yet exist
        Component.onCompleted: {
            if (!exists) {
                makePath(dataPath);
            }
        }
    }
}
