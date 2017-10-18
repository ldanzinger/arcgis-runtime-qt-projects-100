# GeoCode with what3words API

## Background
what3words is a service that aims to geocode the world by assigning a unique combination of 3 words to every location on the Earth. The service grids the Earth into 3m x 3m blocks, and assigns a unique combination of words to that block. A major benefit of this is that it allows for every place on Earth to have a unique address, even if it is not on or near a street. In addition, it is also much more human friendly to share a location by saying 3 words as opposed to telling someone your coordinates in decimal degrees or degrees minutes seconds. For example, compare telling someone that your location is "apple.house.street" to "44.9537° N, 93.0900° W". The what3words API is a REST API that allows you to geocode and reverse geocode, so that you can easily convert between real world coordinates and the what3words location.

## How to use
- Sign up for a developer account [here](https://what3words.com/developers/)
- Obtain an API Key by creating a new application. This can be done by signing into your account > Developer API Keys > Manage Your Applications > Create Application. Once you have done this, copy the API Key to your clipboard
- Paste the API Key into the header so that the m_apiKey member variable contains the key
- Build/Run, and click on the map. The what3words string will appear in the UI.
