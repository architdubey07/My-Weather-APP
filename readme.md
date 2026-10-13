# 🌦️ Real-Time Weather App in C++

A terminal-based weather application built using C++ that fetches current weather information for cities around the world using the Open-Meteo API.

## ✨ Features

- Search for weather by city name
- Current temperature and feels-like temperature
- Relative humidity and wind speed
- Weather condition descriptions
- Precipitation information
- Local time and day/night status
- Error handling for invalid cities and network failures

## 🛠️ Tech Stack

- C++17
- libcurl — HTTP requests
- nlohmann/json — JSON parsing
- Open-Meteo Geocoding API
- Open-Meteo Weather API

## ⚙️ Prerequisites

- A C++17-compatible compiler
- libcurl
- nlohmann/json
- Internet connection

## 🚀 How to Run

Compile the source file using GCC:

```bash
g++ -std=c++17 my_weather_.cpp -o weather.exe -lcurl -lws2_32 -lcrypt32 -lbcrypt
```

Run the application on Windows:

```powershell
.\weather.exe
```

Enter a city name when prompted to retrieve its weather information.

## 🌐 APIs

- [Open-Meteo Weather API](https://open-meteo.com/en/docs)
- [Open-Meteo Geocoding API](https://open-meteo.com/en/docs/geocoding-api)

No API key is required for ordinary non-commercial use.

## 📌 Future Improvements

- Five-day weather forecast
- Automatic weather refresh
- Colored terminal interface
- Search history
- Temperature unit conversion

## 👨‍💻 Author

Archit Dubey

Built as a C++ project to explore API integration, JSON parsing, and command-line application development.