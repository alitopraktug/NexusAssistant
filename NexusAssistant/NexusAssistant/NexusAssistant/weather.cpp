#include "app.h"

string WeatherBetween(const string& s, size_t start, const string& a, const string& b)
{
    size_t p = s.find(a, start);

    if (p == string::npos)
        return "";

    p += a.size();

    size_t q = s.find(b, p);

    if (q == string::npos)
        return "";

    return s.substr(p, q - p);
}

bool IsUrlSafe(char c)
{
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= 'a' && c <= 'z') return true;
    if (c >= '0' && c <= '9') return true;
    if (c == '-' || c == '_' || c == '.' || c == '~') return true;

    return false;
}

string UrlEncode(string s)
{
    string out;

    for (unsigned char c : s)
    {
        if (IsUrlSafe((char)c))
        {
            out += (char)c;
        }
        else if (c == ' ')
        {
            out += "%20";
        }
        else
        {
            char buf[8];
            sprintf_s(buf, "%%%02X", c);
            out += buf;
        }
    }

    return out;
}

wstring ToWideString(string s)
{
    return wstring(s.begin(), s.end());
}

string WeatherHttpGetText(const wchar_t* host, const wstring& path)
{
    string result;

    HINTERNET hSession = WinHttpOpen(
        L"NexusAssistantWeather/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );

    if (!hSession)
        return "";

    HINTERNET hConnect = WinHttpConnect(
        hSession,
        host,
        INTERNET_DEFAULT_HTTPS_PORT,
        0
    );

    if (!hConnect)
    {
        WinHttpCloseHandle(hSession);
        return "";
    }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect,
        L"GET",
        path.c_str(),
        NULL,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );

    if (!hRequest)
    {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return "";
    }

    BOOL ok = WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0
    );

    if (ok)
        ok = WinHttpReceiveResponse(hRequest, NULL);

    if (ok)
    {
        DWORD size = 0;

        do
        {
            size = 0;

            if (!WinHttpQueryDataAvailable(hRequest, &size))
                break;

            if (size == 0)
                break;

            string buffer(size, '\0');
            DWORD downloaded = 0;

            if (!WinHttpReadData(hRequest, &buffer[0], size, &downloaded))
                break;

            buffer.resize(downloaded);
            result += buffer;

        } while (size > 0);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return result;
}

double JsonNumberAfter(const string& s, const string& key)
{
    string pattern = "\"" + key + "\":";

    size_t p = s.find(pattern);

    if (p == string::npos)
        return 0;

    p += pattern.size();

    while (p < s.size() && (s[p] == ' ' || s[p] == '\n' || s[p] == '\r'))
        p++;

    size_t q = p;

    while (q < s.size() && (s[q] == '-' || s[q] == '.' || (s[q] >= '0' && s[q] <= '9')))
        q++;

    if (q <= p)
        return 0;

    return atof(s.substr(p, q - p).c_str());
}

string JsonStringAfter(const string& s, const string& key)
{
    string pattern = "\"" + key + "\":\"";

    size_t p = s.find(pattern);

    if (p == string::npos)
        return "";

    p += pattern.size();

    size_t q = s.find("\"", p);

    if (q == string::npos)
        return "";

    return s.substr(p, q - p);
}

vector<string> JsonArrayValues(const string& s, const string& key)
{
    vector<string> values;

    string pattern = "\"" + key + "\":[";

    size_t p = s.find(pattern);

    if (p == string::npos)
        return values;

    p += pattern.size();

    size_t q = s.find("]", p);

    if (q == string::npos)
        return values;

    string inside = s.substr(p, q - p);
    string item;
    stringstream ss(inside);

    while (getline(ss, item, ','))
    {
        item = Trim(item);

        if (!item.empty() && item.front() == '"')
            item.erase(0, 1);

        if (!item.empty() && item.back() == '"')
            item.erase(item.size() - 1, 1);

        values.push_back(item);
    }

    return values;
}

string WeatherCodeText(int code)
{
    if (code == 0) return "Clear sky";
    if (code == 1 || code == 2 || code == 3) return "Partly cloudy";
    if (code == 45 || code == 48) return "Fog";
    if (code == 51 || code == 53 || code == 55) return "Drizzle";
    if (code == 61 || code == 63 || code == 65) return "Rain";
    if (code == 71 || code == 73 || code == 75) return "Snow";
    if (code == 80 || code == 81 || code == 82) return "Rain showers";
    if (code == 95 || code == 96 || code == 99) return "Thunderstorm";

    return "Weather data";
}

void SaveWeatherCache(string text)
{
    ofstream f("weather_cache.txt");
    f << text;
}

string FetchWeatherText(string city)
{
    city = Trim(city);

    if (city.empty())
        city = cityName;

    string encodedCity = UrlEncode(city);

    string geoPath =
        "/v1/search?name=" + encodedCity +
        "&count=1&language=en&format=json";

    string geoJson = WeatherHttpGetText(
        L"geocoding-api.open-meteo.com",
        ToWideString(geoPath)
    );

    if (geoJson.empty())
        return city + " Weather\n\nConnection failed while searching city.";

    double latitude = JsonNumberAfter(geoJson, "latitude");
    double longitude = JsonNumberAfter(geoJson, "longitude");
    string foundName = JsonStringAfter(geoJson, "name");
    string country = JsonStringAfter(geoJson, "country");

    if (latitude == 0 && longitude == 0)
        return city + " Weather\n\nCity not found.";

    stringstream lat;
    stringstream lon;

    lat << fixed << setprecision(4) << latitude;
    lon << fixed << setprecision(4) << longitude;

    string forecastPath =
        "/v1/forecast?latitude=" + lat.str() +
        "&longitude=" + lon.str() +
        "&current=temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code"
        "&daily=temperature_2m_max,temperature_2m_min,precipitation_probability_max"
        "&forecast_days=3&timezone=auto";

    string forecastJson = WeatherHttpGetText(
        L"api.open-meteo.com",
        ToWideString(forecastPath)
    );

    if (forecastJson.empty())
        return city + " Weather\n\nForecast request failed.";

    string currentBlock = WeatherBetween(forecastJson, 0, "\"current\":{", "}");
    string dailyBlock = forecastJson;

    double temp = JsonNumberAfter(currentBlock, "temperature_2m");
    double humidity = JsonNumberAfter(currentBlock, "relative_humidity_2m");
    double wind = JsonNumberAfter(currentBlock, "wind_speed_10m");
    int code = (int)JsonNumberAfter(currentBlock, "weather_code");

    vector<string> days = JsonArrayValues(dailyBlock, "time");
    vector<string> maxTemps = JsonArrayValues(dailyBlock, "temperature_2m_max");
    vector<string> minTemps = JsonArrayValues(dailyBlock, "temperature_2m_min");
    vector<string> rain = JsonArrayValues(dailyBlock, "precipitation_probability_max");

    stringstream out;
    out << fixed << setprecision(1);

    out << foundName << " Weather";

    if (!country.empty())
        out << " / " << country;

    out << "\n";
    out << "Source: Open-Meteo via WinHTTP\n\n";

    out << "Current Weather\n";
    out << "Temperature: " << temp << " C\n";
    out << "Humidity: " << humidity << "%\n";
    out << "Wind Speed: " << wind << " km/h\n";
    out << "Condition: " << WeatherCodeText(code) << "\n\n";

    out << "3-Day Forecast\n";

    int n = 3;

    if ((int)days.size() < n)
        n = (int)days.size();

    for (int i = 0; i < n; i++)
    {
        out << days[i] << "  ";

        if (i < (int)maxTemps.size())
            out << "Max " << maxTemps[i] << " C  ";

        if (i < (int)minTemps.size())
            out << "Min " << minTemps[i] << " C  ";

        if (i < (int)rain.size())
            out << "Rain " << rain[i] << "%";

        out << "\n";
    }

    out << "\nNexus says:\n";

    if (temp >= 30)
        out << "Hot weather. Drink water and avoid long sun exposure.";
    else if (temp <= 10)
        out << "Cold weather. Wear warm clothes.";
    else
        out << "Weather looks manageable today.";

    string text = out.str();
    SaveWeatherCache(text);

    return text;
}