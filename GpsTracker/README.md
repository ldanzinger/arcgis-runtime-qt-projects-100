# GPS Tracker

This sample demonstrates how to take a GPS Feed and display your location and your tracks on a map.

![](screenshot.png)

## How it works
- A `QGeoPositionInfoSource` is created for obtaining the device's location. An NMEA simulation file is specified for purposes of demonstration.
- The `LocationDisplay` is started on the `MapView`, which takes the information from `QGeoPositionInfoSource`, and display it on a map
- Each location update is saved as a `Point`, and displayed as a `Graphic` on the map.
