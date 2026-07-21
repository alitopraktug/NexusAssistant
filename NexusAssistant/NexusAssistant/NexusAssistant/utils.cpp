#include "app.h"

string Trim(string s)
{
    while (!s.empty() && (s[0] == ' ' || s[0] == '\t'))
        s.erase(0, 1);

    while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r'))
        s.erase(s.size() - 1, 1);

    return s;
}

string Lower(string s)
{
    for (int i = 0; i < (int)s.size(); i++)
        if (s[i] >= 'A' && s[i] <= 'Z')
            s[i] += 32;

    return s;
}

string Today()
{
    time_t now = time(0);
    tm t;
    localtime_s(&t, &now);

    stringstream ss;
    ss << put_time(&t, "%Y-%m-%d");

    return ss.str();
}

string NowText()
{
    time_t now = time(NULL);

    tm t;
    localtime_s(&t, &now);

    char buf[64];

    string mode = Lower(Trim(timeFormatMode));

    if (
        mode == "12" ||
        mode == "12h" ||
        mode == "am" ||
        mode == "pm" ||
        mode == "am/pm" ||
        mode == "ampm"
        )
    {
        strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M %p", &t);
    }
    else
    {
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &t);
    }

    return string(buf);
}

time_t DateToTime(string d)
{
    int y = 0;
    int m = 0;
    int day = 0;

    if (sscanf_s(d.c_str(), "%d-%d-%d", &y, &m, &day) != 3)
        return 0;

    tm t = {};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = day;
    t.tm_hour = 12;

    return mktime(&t);
}

int DaysUntil(string date)
{
    time_t a = DateToTime(Today());
    time_t b = DateToTime(date);

    if (a == 0 || b == 0)
        return 9999;

    return (int)(difftime(b, a) / 86400.0);
}

string GetEdit(int h)
{
    HWND hwnd = ICG_GetHWND(h);

    if (!hwnd)
        return "";

    int len = GetWindowTextLengthA(hwnd);
    vector<char> buf(len + 1);

    GetWindowTextA(hwnd, buf.data(), len + 1);

    return string(buf.data());
}

void SetEditText(int h, string s)
{
    HWND hwnd = ICG_GetHWND(h);

    if (hwnd)
        SetWindowTextA(hwnd, s.c_str());
}

vector<string> Split(string s, char c)
{
    vector<string> v;
    string part;
    stringstream ss(s);

    while (getline(ss, part, c))
        v.push_back(part);

    return v;
}

string Priority(string s)
{
    s = Lower(Trim(s));

    if (s.find("imp") != string::npos || s.find("onem") != string::npos)
        return "important";

    return "normal";
}

string ToStr(double x)
{
    stringstream ss;
    ss << fixed << setprecision(1) << x;

    return ss.str();
}
static WNDPROC oldMainClockProc = NULL;
static string lastTopClockText = "";

void RefreshTopClock()
{
    if (!clockBoxHandle)
        return;

    string now = NowText();

    if (now == lastTopClockText)
        return;

    lastTopClockText = now;

    DrawTop(clockImg, now.c_str(), 300, C_WHITE);
    DisplayImage(clockBoxHandle, clockImg);
}

LRESULT CALLBACK MainClockProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_TIMER && wParam == 7001)
    {
        RefreshTopClock();
        return 0;
    }

    if (oldMainClockProc)
        return CallWindowProc(oldMainClockProc, hwnd, msg, wParam, lParam);

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void InstallClockTimer()
{
    HWND h = ICG_GetMainWindow();

    if (!h)
        return;

    if (!oldMainClockProc)
    {
        oldMainClockProc = (WNDPROC)SetWindowLongPtr(
            h,
            GWLP_WNDPROC,
            (LONG_PTR)MainClockProc
        );
    }

    SetTimer(h, 7001, 1000, NULL);

    RefreshTopClock();
}