# Override Map and Scene Signals

This sample demonstrates how to override the MapView/SceneView/GeoView signals so that you can have custom map interactions. For example, the GeoView is set up to automatically zoom one level on a double click. You could override this behavior so that it does not zoom, but rather pans and centers the map on a mouse double click.

This is accomplished by accepting the QMouseEvent/QWheelEvent:

```
connect(m_mapView, &MapGraphicsView::mouseDoubleClicked, this, [this](QMouseEvent& event)
{
  // accept the event - this will prevent the default behavior from happening
  event.accept();

  // do whatever action you like now, such as zoom to the click location
  const Point p = m_mapView->screenToLocation(event.x(), event.y());
  const Viewpoint vp(p, m_mapView->mapScale());
  m_mapView->setViewpoint(vp, 2.5f);
});
```

This sample is show using the C++ API and Qt Widgets. The same workflow exists for MapQuickView with a QML Interface. The only difference from the above code would be that you connect to `MapQuickView::mouseDoubleClicked` instead of `MapGraphicsView`. If using the QML API, the same concepts exist, but the syntax is slightly different since it is is QML/JS:

```
MapView {
    id: mapView
    anchors.fill: parent

    Map {
        BasemapImageryWithLabels {}            
    }

    onMouseDoubleClicked: {
    	// accept the event - this will prevent the default behavior from happening
        mouse.accepted = true;

        // perform alternative action
        console.log("mouse double clicked");        
    }
}
```
