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

#ifndef GEOCODEWHAT3WORDS_H
#define GEOCODEWHAT3WORDS_H

namespace Esri
{
namespace ArcGISRuntime
{
class Map;
class MapQuickView;
class GraphicsOverlay;
}
}
class QNetworkAccessManager;

#include <QQuickItem>

class GeocodeWhat3Words : public QQuickItem
{
  Q_OBJECT

  Q_PROPERTY(QString what3words READ what3words NOTIFY what3wordsChanged)

public:
  GeocodeWhat3Words(QQuickItem* parent = nullptr);
  ~GeocodeWhat3Words() = default;

  void componentComplete() override;

signals:
  void what3wordsChanged();

private:
  void connectSignals();
  QString what3words() const { return m_what3words; }

private:
  Esri::ArcGISRuntime::Map*             m_map = nullptr;
  Esri::ArcGISRuntime::MapQuickView*    m_mapView = nullptr;
  Esri::ArcGISRuntime::GraphicsOverlay* m_graphicsOverlay = nullptr;
  QNetworkAccessManager*                m_networkManager = nullptr;
  QString                               m_apiKey = ""; // log onto what3words.com and create your own account and application to obtain a key
  QString                               m_urlTemplate = "https://api.what3words.com/v2/reverse?coords=%1,%2&display=full&format=json&key=%3";
  QString                               m_what3words;
};

#endif // GEOCODEWHAT3WORDS_H
