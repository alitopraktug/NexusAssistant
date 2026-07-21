#include "app.h"

Gdiplus::Color GColor(unsigned rgb)
{
    return Gdiplus::Color(
        255,
        (rgb >> 16) & 255,
        (rgb >> 8) & 255,
        rgb & 255
    );
}

wstring W(string s)
{
    return wstring(s.begin(), s.end());
}

void RoundPath(Gdiplus::GraphicsPath& p, float x, float y, float w, float h, float r)
{
    p.AddArc(x, y, r, r, 180, 90);
    p.AddArc(x + w - r, y, r, r, 270, 90);
    p.AddArc(x + w - r, y + h - r, r, r, 0, 90);
    p.AddArc(x, y + h - r, r, r, 90, 90);
    p.CloseFigure();
}

string ShortUpdate(string s)
{
    if (s.empty())
        return "not updated";

    size_t pos = s.find(" ");

    if (pos != string::npos && pos + 1 < s.size())
        return "updated " + s.substr(pos + 1);

    return s;
}

string PortfolioTotalText()
{
    double usdRate = 35.0;
    double euroRate = 38.0;
    double goldRate = 2450.0;

    double total =
        cashTL +
        usdAmount * usdRate +
        euroAmount * euroRate +
        goldGram * goldRate;

    stringstream ss;
    ss << fixed << setprecision(0) << total << " TL";

    return ss.str();
}

string WeatherCardSub()
{
    if (weatherLastUpdate.empty())
        return "not updated";

    return ShortUpdate(weatherLastUpdate);
}

string VehicleCardSub()
{
    long long r = RemainingService();

    if (currentKm <= 0 || lastServiceKm <= 0)
        return "not set";

    if (r > 0)
        return "service remaining";

    return "service due";
}

HBITMAP MakeCardBmp(string title, string value, string sub)
{
    int w = 300;
    int h = 105;

    Gdiplus::Bitmap bmp(w, h, PixelFormat32bppARGB);
    Gdiplus::Graphics g(&bmp);

    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.Clear(GColor(0x080C14));

    Gdiplus::GraphicsPath path;
    RoundPath(path, 2.0f, 2.0f, (float)w - 4.0f, (float)h - 4.0f, 18.0f);

    Gdiplus::Rect gradRect(0, 0, w, h);

    Gdiplus::LinearGradientBrush bg(
        gradRect,
        GColor(0x151B2A),
        GColor(0x28120B),
        Gdiplus::LinearGradientModeForwardDiagonal
    );

    g.FillPath(&bg, &path);

    Gdiplus::Pen border(GColor(0x00E5FF), 1.4f);
    g.DrawPath(&border, &path);

    Gdiplus::SolidBrush cyan(GColor(0x00E5FF));
    Gdiplus::SolidBrush yellow(GColor(0xFFE500));
    Gdiplus::SolidBrush white(GColor(0xFFFFFF));
    Gdiplus::SolidBrush muted(GColor(0xAAB7C4));

    Gdiplus::FontFamily ff(L"Segoe UI");

    Gdiplus::Font fTitle(
        &ff,
        11,
        Gdiplus::FontStyleBold,
        Gdiplus::UnitPixel
    );

    Gdiplus::Font fValue(
        &ff,
        24,
        Gdiplus::FontStyleBold,
        Gdiplus::UnitPixel
    );

    Gdiplus::Font fSub(
        &ff,
        12,
        Gdiplus::FontStyleRegular,
        Gdiplus::UnitPixel
    );

    Gdiplus::GraphicsPath pill;
    RoundPath(pill, 16.0f, 18.0f, 8.0f, 32.0f, 5.0f);
    g.FillPath(&yellow, &pill);

    wstring wt = W(title);
    wstring wv = W(value);
    wstring ws = W(sub);

    g.DrawString(wt.c_str(), -1, &fTitle, Gdiplus::PointF(38.0f, 17.0f), &cyan);
    g.DrawString(wv.c_str(), -1, &fValue, Gdiplus::PointF(38.0f, 42.0f), &white);
    g.DrawString(ws.c_str(), -1, &fSub, Gdiplus::PointF(38.0f, 76.0f), &muted);

    HBITMAP out = 0;
    bmp.GetHBITMAP(GColor(0x080C14), &out);

    return out;
}

void CardGo(int i)
{
    if (i == 0)
        ShowSchedule();

    if (i == 1)
        ShowNotifications();

    if (i == 2)
        ShowNotifications();

    if (i == 3)
        ShowWeather();

    if (i == 4)
        ShowVehicle();

    if (i == 5)
        ShowFinance();

    if (i == 6)
        ShowPortfolio();

    if (i == 7)
        ShowWorkout();

    if (i == 8)
        ShowSports();
}

LRESULT CALLBACK CardProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int i = (int)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    if (msg == WM_LBUTTONUP)
    {
        CardGo(i);
        return 0;
    }

    if (msg == WM_SETCURSOR)
    {
        SetCursor(LoadCursor(NULL, IDC_HAND));
        return TRUE;
    }

    if (i >= 0 && i < 9 && cardOldProc[i])
        return CallWindowProc(cardOldProc[i], hwnd, msg, wParam, lParam);

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void SetCard(int i, string title, string value, string sub)
{
    int x[9] = {
        335, 655, 975,
        335, 655, 975,
        335, 655, 975
    };

    int y[9] = {
        135, 135, 135,
        265, 265, 265,
        395, 395, 395
    };

    HBITMAP bmp = MakeCardBmp(title, value, sub);

    if (!cardWnd[i])
    {
        cardWnd[i] = CreateWindowExA(
            0,
            "STATIC",
            "",
            WS_CHILD | WS_VISIBLE | SS_BITMAP | SS_NOTIFY,
            x[i],
            y[i],
            300,
            105,
            ICG_GetMainWindow(),
            NULL,
            GetInstance(),
            NULL
        );

        SetWindowLongPtr(cardWnd[i], GWLP_USERDATA, i);

        cardOldProc[i] = (WNDPROC)SetWindowLongPtr(
            cardWnd[i],
            GWLP_WNDPROC,
            (LONG_PTR)CardProc
        );
    }

    HBITMAP old = (HBITMAP)SendMessage(
        cardWnd[i],
        STM_SETIMAGE,
        IMAGE_BITMAP,
        (LPARAM)bmp
    );

    if (old)
        DeleteObject(old);

    cardBmp[i] = bmp;

    SetWindowPos(
        cardWnd[i],
        HWND_TOP,
        x[i],
        y[i],
        300,
        105,
        SWP_SHOWWINDOW
    );

    InvalidateRect(cardWnd[i], NULL, TRUE);
    UpdateWindow(cardWnd[i]);
}

void ShowDashboardCards(bool show)
{
    for (int i = 0; i < 9; i++)
    {
        if (!cardWnd[i])
            continue;

        ShowWindow(cardWnd[i], show ? SW_SHOW : SW_HIDE);

        if (show)
        {
            SetWindowPos(
                cardWnd[i],
                HWND_TOP,
                0,
                0,
                0,
                0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW
            );

            InvalidateRect(cardWnd[i], NULL, TRUE);
            UpdateWindow(cardWnd[i]);
        }
    }
}

void UpdateDashboardCards()
{
    stringstream today;
    stringstream important;
    stringstream notif;
    stringstream vehicle;
    stringstream finance;
    stringstream workout;

    today << TodayTaskCount() << " event";
    important << ImportantCount() << " important";
    notif << NotificationCount() << " alert";

    long long remaining = RemainingService();

    if (currentKm <= 0 || lastServiceKm <= 0)
        vehicle << "not set";
    else
        vehicle << remaining << " km";

    finance << fixed << setprecision(0) << Balance() << " TL";
    workout << fixed << setprecision(1) << BMI();

    string weatherValue = cityName;
    string weatherSub = WeatherCardSub();

    SetCard(0, "TODAY", today.str(), NextTask());
    SetCard(1, "IMPORTANT", important.str(), "priority items");
    SetCard(2, "NOTIFICATIONS", notif.str(), "open alerts");

    SetCard(3, "WEATHER", weatherValue, weatherSub);
    SetCard(4, "VEHICLE", vehicle.str(), VehicleCardSub());
    SetCard(5, "FINANCE", finance.str(), "current balance");

    SetCard(6, "PORTFOLIO", PortfolioTotalText(), "estimated total");
    SetCard(7, "WORKOUT", workout.str(), BMIStatus());
    SetCard(8, "SPORTS", "TUR vs PAR", "featured match");

    ShowDashboardCards(currentPage == DASHBOARD);
}

void DrawTop(ICBYTES& img, const char* text, int w, unsigned color)
{
    CreateImage(img, w, 52, ICB_UINT);

    FillRoundRect(img, 0, 0, w, 52, 15, C_PANEL);

    Line(img, 15, 1, w - 16, 1, color);
    Line(img, 15, 50, w - 16, 50, C_BORDER);
    Line(img, 1, 15, 1, 37, C_BORDER);
    Line(img, w - 2, 15, w - 2, 37, color);

    DrawTextWIN(img, 20, 13, text, color, 23, 0, "Consolas");
}

void DrawPanel(ICBYTES& img)
{
    CreateImage(img, 1035, 650, ICB_UINT);

    FillRoundRect(img, 0, 0, 1035, 650, 20, C_PANEL);

    Line(img, 18, 1, 1017, 1, C_CYAN);
    Line(img, 18, 648, 1017, 648, C_BORDER);
    Line(img, 1, 18, 1, 632, C_BORDER);
    Line(img, 1033, 18, 1033, 632, C_CYAN);
}

void DrawButton(ICBYTES& img, const char* text, bool active)
{
    CreateImage(img, 210, 39, ICB_UINT);

    unsigned bg = active ? C_ACTIVE : C_DARK;
    unsigned edge = active ? C_CYAN : C_BORDER;

    FillRoundRect(img, 0, 0, 210, 39, 12, bg);

    Line(img, 12, 1, 198, 1, edge);
    Line(img, 12, 37, 198, 37, C_BORDER);
    Line(img, 1, 12, 1, 27, C_BORDER);
    Line(img, 208, 12, 208, 27, edge);

    FillRoundRect(img, 9, 10, 5, 20, 3, edge);

    DrawTextWIN(
        img,
        28,
        9,
        text,
        active ? C_CYAN : C_WHITE,
        18,
        0,
        "Consolas"
    );
}

void DrawRun(ICBYTES& img)
{
    CreateImage(img, 205, 40, ICB_UINT);

    FillRoundRect(img, 0, 0, 205, 40, 12, C_ACTIVE);

    Line(img, 12, 1, 192, 1, C_CYAN);
    Line(img, 12, 38, 192, 38, C_BORDER);
    Line(img, 1, 12, 1, 28, C_BORDER);
    Line(img, 203, 12, 203, 28, C_CYAN);

    DrawTextWIN(img, 78, 9, "Run", C_CYAN, 20, 0, "Consolas");
}