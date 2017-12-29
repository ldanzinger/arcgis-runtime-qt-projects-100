// [WriteFile Name=LineWithArrow, Category=DisplayInformation]
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

#include "LineWithArrow.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "Graphic.h"
#include "SimpleLineSymbol.h"
#include "PolylineBuilder.h"
#include "Point.h"
#include "SpatialReference.h"

using namespace Esri::ArcGISRuntime;

LineWithArrow::LineWithArrow(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void LineWithArrow::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<LineWithArrow>("Esri.Samples", 1, 0, "LineWithArrowSample");
}

void LineWithArrow::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create the overlay and geometry
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
  PolylineBuilder* builder = new PolylineBuilder(SpatialReference::wgs84(), this);
  builder->addPoint(-45,-25);
  builder->addPoint(45,0);
  builder->addPoint(-45, 25);

  // Create the symbol and specify to show arrow markers
  m_lineSymbol = new SimpleLineSymbol(SimpleLineSymbolStyle::Solid, QColor("red"), 3.0f, this);
  m_lineSymbol->setMarkerStyle(SimpleLineSymbolMarkerStyle::Arrow);
  m_lineSymbol->setMarkerPlacement(SimpleLineSymbolMarkerPlacement::Begin);

  // Create and add the graphic
  Graphic* graphic = new Graphic(builder->toGeometry(), m_lineSymbol, this);
  m_graphicsOverlay->graphics()->append(graphic);
}

void LineWithArrow::updateArrow(const QString& placement)
{
  if (!m_lineSymbol)
    return;

  if (placement.toLower() == "beginning")
    m_lineSymbol->setMarkerPlacement(SimpleLineSymbolMarkerPlacement::Begin);
  else if (placement.toLower() == "end")
    m_lineSymbol->setMarkerPlacement(SimpleLineSymbolMarkerPlacement::End);
  else if (placement.toLower() == "both")
    m_lineSymbol->setMarkerPlacement(SimpleLineSymbolMarkerPlacement::BeginAndEnd);
}
