// [WriteFile Name=GpsTracker, Category=EditData]
// [Legal]
// Copyright 2017 Esri.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// [Legal]

#include "GpsTracker.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "SimpleLineSymbol.h"
#include "PolylineBuilder.h"
#include "SimpleRenderer.h"
#include <QGeoPositionInfoSource>
#include <QNmeaPositionInfoSource>
#include <QFile>

using namespace Esri::ArcGISRuntime;

GpsTracker::GpsTracker(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void GpsTracker::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GpsTracker>("Esri.Samples", 1, 0, "GpsTrackerSample");
}

void GpsTracker::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create the position info source
  m_mapView->locationDisplay()->setPositionSource(QGeoPositionInfoSource::createDefaultSource(this));
  m_mapView->locationDisplay()->start();
  QFile* logFile = new QFile(":/GpsTracker/campus.txt", this);
  QNmeaPositionInfoSource* nmeaSource = new QNmeaPositionInfoSource(
        QNmeaPositionInfoSource::SimulationMode, this);
  nmeaSource->setDevice(logFile);
  nmeaSource->setUpdateInterval(500);
  connect(nmeaSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
          this, SLOT(positionUpdated(QGeoPositionInfo)));
  nmeaSource->startUpdates();

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Setup the GraphicsOverlay to display the tracks
  GraphicsOverlay* overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(overlay);
  SimpleLineSymbol* sls = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 4.0, this);
  SimpleRenderer* renderer = new SimpleRenderer(sls, this);
  overlay->setRenderer(renderer);
  m_polylineBuilder = new PolylineBuilder(SpatialReference::wgs84(), this);
  m_graphic = new Graphic(m_polylineBuilder->toGeometry(), this);
  overlay->graphics()->append(m_graphic);

  // start the location display
  m_mapView->locationDisplay()->setPositionSource(nmeaSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->start();
}

void GpsTracker::positionUpdated(QGeoPositionInfo positionInfo)
{
  double y = positionInfo.coordinate().latitude();
  double x = positionInfo.coordinate().longitude();
  m_polylineBuilder->addPoint(x,y);
  m_graphic->setGeometry(m_polylineBuilder->toGeometry());
}
