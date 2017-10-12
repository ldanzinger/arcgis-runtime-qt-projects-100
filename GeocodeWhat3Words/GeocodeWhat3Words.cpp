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

#include "Map.h"
#include "MapQuickView.h"
#include "Basemap.h"
#include "Point.h"
#include "GeometryEngine.h"
#include "Graphic.h"
#include "GraphicsOverlay.h"
#include "SimpleRenderer.h"
#include "SimpleMarkerSymbol.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include "GeocodeWhat3Words.h"

using namespace Esri::ArcGISRuntime;

GeocodeWhat3Words::GeocodeWhat3Words(QQuickItem* parent /* = nullptr */):
  QQuickItem(parent)
{
}

void GeocodeWhat3Words::componentComplete()
{
  QQuickItem::componentComplete();

  // find QML MapView component
  m_mapView = findChild<MapQuickView*>("mapView");

  // Create a map using the topographic BaseMap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // Create the GraphicsOverlay
  m_graphicsOverlay = new GraphicsOverlay(this);
  m_mapView->graphicsOverlays()->append(m_graphicsOverlay);
  SimpleMarkerSymbol* sms = new SimpleMarkerSymbol(SimpleMarkerSymbolStyle::Circle, QColor("red"), 15.0f, this);
  SimpleRenderer* sr = new SimpleRenderer(sms, this);
  m_graphicsOverlay->setRenderer(sr);

  // create the network manager
  m_networkManager = new QNetworkAccessManager(this);

  // connect signals
  connectSignals();
}

void GeocodeWhat3Words::connectSignals()
{
  // connect to mouse click
  connect(m_mapView, &MapQuickView::mouseClicked, this, [this](QMouseEvent& e)
  {
    // get the point in WGS1984 and add to the map
    Point p = GeometryEngine::project(m_mapView->screenToLocation(e.x(), e.y()), SpatialReference::wgs84());
    m_graphicsOverlay->graphics()->clear();
    m_graphicsOverlay->graphics()->append(new Graphic(p, this));

    // Reverse geocode for the words - https://docs.what3words.com/api/v2/#reverse
    QNetworkRequest request(QUrl(m_urlTemplate.arg(p.x()).arg(p.y()).arg(m_apiKey)));
    m_networkManager->get(request);
  });

  // connect to the network reply
  connect(m_networkManager, &QNetworkAccessManager::finished, this, [this](QNetworkReply* reply)
  {
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
      // Get the http status code
      int code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
      if (code >= 200 && code < 300) // Success
      {
        // Here we got the final reply
        QString replyText = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(replyText.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();
        m_what3words = jsonObj["words"].toString();
        emit what3wordsChanged();
      }
    }
    else
    {
      // Error
      qDebug() << reply->errorString();
    }
  });
}
