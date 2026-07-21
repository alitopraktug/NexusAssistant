#include "app.h"

struct SportsGame {
    string name;
    string date;
    string state;
    string score;
    bool turkiye;
};

bool HasText(string s, string key)
{
    return s.find(key) != string::npos;
}

string BetweenText(const string& s, size_t start, const string& a, const string& b)
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

string HttpGetText(const wchar_t* host, const wchar_t* path)
{
    string result;

    HINTERNET hSession = WinHttpOpen(
        L"NexusAssistant/1.0",
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
        path,
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

vector<SportsGame> ParseEspnGames(string json)
{
    vector<SportsGame> games;
    size_t pos = 0;

    while ((pos = json.find("\"shortName\":\"", pos)) != string::npos)
    {
        string name = BetweenText(json, pos, "\"shortName\":\"", "\"");

        size_t next = json.find("\"shortName\":\"", pos + 13);
        string block;

        if (next == string::npos)
            block = json.substr(pos);
        else
            block = json.substr(pos, next - pos);

        string date = BetweenText(block, 0, "\"date\":\"", "\"");
        string state = BetweenText(block, 0, "\"state\":\"", "\"");

        vector<string> scores;
        size_t sp = 0;

        while ((sp = block.find("\"score\":\"", sp)) != string::npos)
        {
            string sc = BetweenText(block, sp, "\"score\":\"", "\"");

            if (!sc.empty())
                scores.push_back(sc);

            sp += 9;
        }

        string score = "";

        if (scores.size() >= 2)
            score = scores[0] + "-" + scores[1];

        SportsGame g;
        g.name = name;
        g.date = date;
        g.state = state;
        g.score = score;
        g.turkiye = HasText(name, "TUR") || HasText(name, "Turkey") || HasText(name, "Turkiye");

        if (!g.name.empty())
            games.push_back(g);

        pos += 13;

        if (games.size() > 80)
            break;
    }

    return games;
}

bool IsPlayed(SportsGame g)
{
    return g.state == "post" || g.state == "final";
}

bool IsUpcoming(SportsGame g)
{
    return g.state == "pre" || g.state == "scheduled";
}

string GameLine(SportsGame g)
{
    if (!g.score.empty() && IsPlayed(g))
        return g.name + " | " + g.score;

    if (!g.date.empty())
        return g.name + " | " + g.date;

    return g.name;
}

vector<SportsGame> PickGames(vector<SportsGame> v, int count)
{
    vector<SportsGame> out;

    if (v.empty())
        return out;

    int start = (int)(time(0) % v.size());

    for (int i = 0; i < count && i < (int)v.size(); i++)
        out.push_back(v[(start + i) % v.size()]);

    return out;
}

string StaticSportsText()
{
    return
        "World Cup 2026 Sports Panel\n\n"
        "Played World Cup Games\n"
        "Mexico 2-0 South Africa\n"
        "South Korea 2-1 Czechia\n"
        "United States 4-1 Paraguay\n"
        "Germany 7-1 Curacao\n\n"
        "Fixed Played Turkiye Game\n"
        "Australia 2-0 Turkiye\n\n"
        "Upcoming World Cup Games\n"
        "France vs Senegal\n"
        "Iraq vs Norway\n"
        "Argentina vs Algeria\n"
        "England vs Croatia\n\n"
        "Fixed Upcoming Turkiye Game\n"
        "Turkiye vs Paraguay\n\n"
        "Nexus says:\n"
        "Press Refresh to fetch sports data with WinHTTP.";
}

void SaveSportsCache(string text)
{
    ofstream f("sports_cache.txt");
    f << text;
}

string LoadSportsCache()
{
    ifstream f("sports_cache.txt");
    stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

string BuildSportsText(vector<SportsGame> games)
{
    vector<SportsGame> played;
    vector<SportsGame> upcoming;
    vector<SportsGame> turPlayed;
    vector<SportsGame> turUpcoming;

    for (SportsGame& g : games)
    {
        if (g.turkiye && IsPlayed(g))
            turPlayed.push_back(g);
        else if (!g.turkiye && IsPlayed(g))
            played.push_back(g);

        if (g.turkiye && IsUpcoming(g))
            turUpcoming.push_back(g);
        else if (!g.turkiye && IsUpcoming(g))
            upcoming.push_back(g);
    }

    vector<SportsGame> randomUpcoming = PickGames(upcoming, 4);

    string s = "World Cup 2026 Sports Panel\n";
    s += "Source: ESPN scoreboard via WinHTTP\n\n";

    s += "Played World Cup Games\n";

    if (played.empty())
    {
        s += "No played games found.\n";
    }
    else
    {
        for (int i = 0; i < 4 && i < (int)played.size(); i++)
            s += GameLine(played[i]) + "\n";
    }

    s += "\nFixed Played Turkiye Game\n";

    if (!turPlayed.empty())
        s += GameLine(turPlayed[0]) + "\n";
    else
        s += "Australia 2-0 Turkiye\n";

    s += "\nRandom Upcoming World Cup Games\n";

    if (randomUpcoming.empty())
    {
        s += "No upcoming games found.\n";
    }
    else
    {
        for (SportsGame& g : randomUpcoming)
            s += GameLine(g) + "\n";
    }

    s += "\nFixed Upcoming Turkiye Game\n";

    if (!turUpcoming.empty())
        s += GameLine(turUpcoming[0]) + "\n";
    else
        s += "Turkiye vs Paraguay\n";

    s += "\nNexus says:\n";
    s += "Sports data fetched and cached in sports_cache.txt.";

    return s;
}

string FetchSportsText()
{
    string json = HttpGetText(
        L"site.api.espn.com",
        L"/apis/site/v2/sports/soccer/fifa.world/scoreboard?limit=100"
    );

    if (json.empty())
        return StaticSportsText() + "\n\nConnection failed. Static data shown.";

    vector<SportsGame> games = ParseEspnGames(json);

    if (games.empty())
        return StaticSportsText() + "\n\nData fetched but parser found no games.";

    string text = BuildSportsText(games);
    SaveSportsCache(text);

    return text;
}

string SportsText()
{
    string cache = LoadSportsCache();

    if (!cache.empty())
        return cache + "\n\nPress Refresh to update sports data.";

    return StaticSportsText();
}