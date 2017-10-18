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

#include "OverrideMapSignals.h"

#include "Basemap.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "Point.h"
#include "Viewpoint.h"

using namespace Esri::ArcGISRuntime;

OverrideMapSignals::OverrideMapSignals(QWidget* parent /*=nullptr*/):
  QMainWindow(parent)
{
  // Create the Widget view
  m_mapView = new MapGraphicsView(this);

  // Create a map using the topographic BaseMap
  m_map = new Map(Basemap::topographic(this), this);

  // Set map to map view
  m_mapView->setMap(m_map);

  // set the mapView as the central widget
  setCentralWidget(m_mapView);

  // override the double click to center instead of zoom
  connect(m_mapView, &MapGraphicsView::mouseDoubleClicked, this, [this](QMouseEvent& event)
  {
    // accept the event - this will prevent the default behavior from happening
    event.accept();

    // do whatever action you like now, such as zoom to the click location
    const Point p = m_mapView->screenToLocation(event.x(), event.y());
    const Viewpoint vp(p, m_mapView->mapScale());
    m_mapView->setViewpoint(vp, 2.5f);
  });
}
