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

#include "GeoFence.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "SimpleFillSymbol.h"
#include "SimpleRenderer.h"
#include "Point.h"
#include "GeometryEngine.h"
#include <QGeoPositionInfoSource>
#include <QNmeaPositionInfoSource>
#include <QFile>

using namespace Esri::ArcGISRuntime;

GeoFence::GeoFence(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void GeoFence::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<GeoFence>("Esri.Samples", 1, 0, "GeoFenceSample");
}

void GeoFence::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // create the position info source
  m_mapView->locationDisplay()->setPositionSource(QGeoPositionInfoSource::createDefaultSource(this));
  m_mapView->locationDisplay()->start();
  QFile* logFile = new QFile(":/GeoFence/campus.txt", this);
  QNmeaPositionInfoSource* nmeaSource = new QNmeaPositionInfoSource(
        QNmeaPositionInfoSource::SimulationMode, this);
  nmeaSource->setDevice(logFile);
  nmeaSource->setUpdateInterval(500);
  //  connect(nmeaSource, SIGNAL(positionUpdated(QGeoPositionInfo)),
  //          this, SLOT(positionUpdated(QGeoPositionInfo)));
  nmeaSource->startUpdates();

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Setup the GraphicsOverlay to display the tracks
  GraphicsOverlay* overlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(overlay);
  SimpleFillSymbol* sfs = new SimpleFillSymbol(SimpleFillSymbolStyle::Solid, QColor("cyan"), this);
  SimpleRenderer* renderer = new SimpleRenderer(sfs, this);
  overlay->setRenderer(renderer);
  Polygon alertZone = Polygon::fromJson("{\"rings\":[[[-13046212.520199999,4036277.4038000032],[-13046212.520199999,4036650.7616999969],[-13046176.962400001,4036650.7616999969],[-13046176.962400001,4036277.4038000032],[-13046212.520199999,4036277.4038000032]]],\"spatialReference\":{\"wkid\":102100,\"latestWkid\":3857}}");
  m_graphic = new Graphic(alertZone, this);
  overlay->graphics()->append(m_graphic);

  // start the location display
  m_mapView->locationDisplay()->setPositionSource(nmeaSource);
  m_mapView->locationDisplay()->setAutoPanMode(LocationDisplayAutoPanMode::Recenter);
  m_mapView->locationDisplay()->start();

  // alert when the current location intersects the polygon graphic
  connect(m_mapView->locationDisplay(), &LocationDisplay::locationChanged, this, [this](Location location)
  {
    Point currentLocation = GeometryEngine::project(location.position(), SpatialReference::webMercator());
    m_within = GeometryEngine::intersects(currentLocation, m_graphic->geometry());
    if (!(m_previouslyWithin) && m_within) {
      m_previouslyWithin = true;
      emit sendAlert("Hello");
    } else if (m_previouslyWithin && !(m_within)) {
      m_previouslyWithin = false;
      emit sendAlert("Goodbye");
    } else if (m_previouslyWithin && m_within) {
      // they were previously within, and still are
      m_previouslyWithin = true;
      return;
    } else {
      // they were not previously within, and still are not
      return;
    }
  });
}
