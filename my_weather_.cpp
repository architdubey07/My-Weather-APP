
#include <iostream>
#include <string>
#include <iomanip>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

// Store HTTP response data
size_t WriteCallback(void* contents, size_t size,
                     size_t nmemb, string* output) {
    output->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

// Make an HTTP GET request
string httpGet(const string& url) {
    CURL* curl = curl_easy_init();
    string response;

    if (!curl) {
        throw runtime_error("Could not initialize CURL");
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);

    CURLcode result = curl_easy_perform(curl);
    long statusCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);

    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        throw runtime_error(curl_easy_strerror(result));
    }

    if (statusCode < 200 || statusCode >= 300) {
        throw runtime_error("HTTP error: " + to_string(statusCode));
    }

    return response;
}

// Convert weather codes to readable descriptions
string weatherDescription(int code) {
    if (code == 0) return "Clear sky";
    if (code == 1) return "Mainly clear";
    if (code == 2) return "Partly cloudy";
    if (code == 3) return "Overcast";
    if (code == 45 || code == 48) return "Fog";
    if (code >= 51 && code <= 57) return "Drizzle";
    if (code >= 61 && code <= 67) return "Rain";
    if (code >= 71 && code <= 77) return "Snowfall";
    if (code >= 80 && code <= 82) return "Rain showers";
    if (code == 85 || code == 86) return "Snow showers";
    if (code >= 95) return "Thunderstorm";
    return "Unknown conditions";
}

// Search for city coordinates
json getCity(const string& city) {
    CURL* curl = curl_easy_init();

    if (!curl) {
        throw runtime_error("Could not initialize CURL");
    }

    char* encoded = curl_easy_escape(
        curl, city.c_str(), static_cast<int>(city.size())
    );

    if (!encoded) {
        curl_easy_cleanup(curl);
        throw runtime_error("Could not encode city name");
    }

    string url =
        "https://geocoding-api.open-meteo.com/v1/search?name=" +
        string(encoded) + "&count=1&language=en&format=json";

    curl_free(encoded);
    curl_easy_cleanup(curl);

    json data = json::parse(httpGet(url));

    if (!data.contains("results") ||
        data["results"].empty()) {
        throw runtime_error("City not found. Try another name.");
    }

    return data["results"][0];
}

// Fetch current weather
json getWeather(double latitude, double longitude) {
    string url =
        "https://api.open-meteo.com/v1/forecast"
        "?latitude=" + to_string(latitude) +
        "&longitude=" + to_string(longitude) +
        "&current=temperature_2m,relative_humidity_2m,"
        "apparent_temperature,is_day,precipitation,"
        "weather_code,wind_speed_10m"
        "&timezone=auto";

    json data = json::parse(httpGet(url));

    if (!data.contains("current")) {
        throw runtime_error("Current weather data unavailable.");
    }

    return data["current"];
}

// Display weather dashboard
void displayWeather(const json& city, const json& weather) {
    const auto& current = weather;

    cout << "\n";
    cout << "========================================\n";
    cout << "          REAL-TIME WEATHER\n";
    cout << "========================================\n";

    cout << "City       : " << city.value("name", "Unknown") << "\n";
    cout << "Region     : " << city.value("admin1", "N/A") << "\n";
    cout << "Country    : " << city.value("country", "N/A") << "\n";
    cout << "Local time : " << current.value("time", "N/A") << "\n";

    cout << fixed << setprecision(1);

    cout << "----------------------------------------\n";
    cout << "Temperature: "
         << current.at("temperature_2m").get<double>() << " C\n";

    cout << "Feels like : "
         << current.at("apparent_temperature").get<double>() << " C\n";

    cout << "Condition  : "
         << weatherDescription(current.at("weather_code").get<int>())
         << "\n";

    cout << "Humidity   : "
         << current.at("relative_humidity_2m").get<double>() << " %\n";

    cout << "Wind speed : "
         << current.at("wind_speed_10m").get<double>() << " km/h\n";

    cout << "Precip.    : "
         << current.at("precipitation").get<double>() << " mm\n";

    cout << "----------------------------------------\n";

    cout << "Time of day: "
         << (current.at("is_day").get<int>() ? "Day" : "Night")
         << "\n";

    cout << "========================================\n";
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);

    cout << "========================================\n";
    cout << "       C++ WEATHER APPLICATION\n";
    cout << "========================================\n";

    while (true) {
        cout << "\nEnter city name (or 'exit' to quit): ";

        string cityName;
        getline(cin, cityName);

        if (cityName == "exit" || cityName == "EXIT") {
            break;
        }

        if (cityName.empty()) {
            cout << "Please enter a city name.\n";
            continue;
        }

        try {
            json city = getCity(cityName);

            json weather = getWeather(
                city.at("latitude").get<double>(),
                city.at("longitude").get<double>()
            );

            displayWeather(city, weather);

        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << "\n";
        }
    }

    curl_global_cleanup();

    cout << "\nWeather app closed. Goodbye!\n";
    return 0;
}
