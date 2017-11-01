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

#ifndef CUSTOMSYMBOLS_H
#define CUSTOMSYMBOLS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
class Graphic;
class SymbolStyle;
class SimpleRenderer;
class PolylineBuilder;
}
}

#include <QQuickItem>

class CustomSymbols : public QQuickItem
{
  Q_OBJECT

public:
  explicit CustomSymbols(QQuickItem* parent = nullptr);
  ~CustomSymbols() = default;

  void componentComplete() Q_DECL_OVERRIDE;
  static void init();

  Q_INVOKABLE void clear();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  Esri::ArcGISRuntime::SymbolStyle* m_symbolStyle = nullptr;
  Esri::ArcGISRuntime::SimpleRenderer* m_simpleRenderer = nullptr;
  Esri::ArcGISRuntime::PolylineBuilder* m_polylineBuilder = nullptr;
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;
};

#endif // CUSTOMSYMBOLS_H
