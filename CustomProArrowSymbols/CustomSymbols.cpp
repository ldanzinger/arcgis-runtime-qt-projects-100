// [WriteFile Name=CustomSymbols, Category=Analysis]
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

#include "CustomSymbols.h"

#include "Map.h"
#include "MapQuickView.h"
#include "GraphicsOverlay.h"
#include "SimpleRenderer.h"
#include "PolylineBuilder.h"
#include "SymbolStyle.h"
#include "Graphic.h"
#include "GeometryEngine.h"

using namespace Esri::ArcGISRuntime;

CustomSymbols::CustomSymbols(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void CustomSymbols::init()
{
  // Register the map view for QML
  qmlRegisterType<MapQuickView>("Esri.Samples", 1, 0, "MapView");
  qmlRegisterType<CustomSymbols>("Esri.Samples", 1, 0, "CustomSymbolsSample");
}

void CustomSymbols::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic basemap
  m_map = new Map(Basemap::topographic(this), this);

  // Add a Graphics Overlay with a Simple Renderer
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_simpleRenderer = new SimpleRenderer(this);
  m_graphicsOverlay->setRenderer(m_simpleRenderer);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
  m_polylineBuilder = new PolylineBuilder(SpatialReference(3857), this);
  m_graphic = new Graphic(this);
  m_graphicsOverlay->graphics()->append(m_graphic);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Setup the Symbol Style
  m_symbolStyle = new SymbolStyle("/Users/<username>/Desktop/RuntimeStyles.stylx", this); // TODO - Update with your style file

  // connect to symbol style signal
  connect(m_symbolStyle, &SymbolStyle::fetchSymbolCompleted, this, [this](QUuid, Symbol* symbol)
  {
    m_simpleRenderer->setSymbol(symbol);
  });

  // fetch the custom symbol
  m_symbolStyle->fetchSymbol(QStringList{"RuntimeArrow"}); // TODO - Update with your unique Key

  // connect to mouse clicked
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& event)
  {
    Point pt = m_mapView->screenToLocation(event.x(), event.y());
    m_polylineBuilder->addPoint(pt);
    if (m_polylineBuilder->toPolyline().parts().part(0).points().size()  < 2)
      return;

    m_graphic->setGeometry(m_polylineBuilder->toGeometry());
  });
}

void CustomSymbols::clear()
{
  m_graphic->setGeometry(Point());
  m_polylineBuilder->replaceGeometry(Geometry());
}
