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

#ifndef GPSTRACKER_H
#define GPSTRACKER_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class Graphic;
class PolylineBuilder;
}
}

class QNmeaPositionInfoSource;

#include <QQuickItem>
#include <QGeoPositionInfo>

class GpsTracker : public QQuickItem
{
  Q_OBJECT

public:
  explicit GpsTracker(QQuickItem* parent = nullptr);
  ~GpsTracker() = default;

  void componentComplete() Q_DECL_OVERRIDE;
  static void init();

private:
  Esri::ArcGISRuntime::Map* m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView* m_mapView = nullptr;
  QNmeaPositionInfoSource* m_nmeaSource;
  Esri::ArcGISRuntime::PolylineBuilder* m_polylineBuilder = nullptr;
  Esri::ArcGISRuntime::Graphic* m_graphic = nullptr;

private slots:
  void positionUpdated(QGeoPositionInfo positionInfo);
};

#endif // GPSTRACKER_H
