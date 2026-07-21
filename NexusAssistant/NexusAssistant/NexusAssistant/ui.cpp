#include "app.h"

const char* menuNames[MENU_COUNT] = {
    "Dashboard",
    "Schedule",
    "Notifications",
    "Special Days",
    "Sports",
    "Weather",
    "Vehicle",
    "Finance",
    "Portfolio",
    "Workout",
    "AI Assistant",
    "Settings"
};

void (*menuCallbacks[MENU_COUNT])() = {
    ShowDashboard,
    ShowSchedule,
    ShowNotifications,
    ShowSpecial,
    ShowSports,
    ShowWeather,
    ShowVehicle,
    ShowFinance,
    ShowPortfolio,
    ShowWorkout,
    ShowAI,
    ShowSettings
};

void BuildUI()
{
    LoadAll();

    DrawTop(logoImg, "NEXUS ASSISTANT", 430, C_CYAN);
    int logo = ICG_Frameless(25, 18, 430, 52);
    DisplayImage(logo, logoImg);

    string now = NowText();
    DrawTop(clockImg, NowText().c_str(), 300, C_WHITE);
    clockBoxHandle = ICG_Frameless(1080, 45, 300, 52);
    DisplayImage(clockBoxHandle, clockImg);

    int x = 35;
    int y = 100;

    for (int i = 0; i < MENU_COUNT; i++)
    {
        DrawButton(menuImgs[i], menuNames[i], i == 0);
        menuHandles[i] = ICG_BitmapButton(x, y, 210, 39, menuCallbacks[i]);
        SetButtonBitmap(menuHandles[i], menuImgs[i]);
        y += 46;
    }

    UpdateDashboardCards();

    titleBox = ICG_Frameless(335, 130, 940, 38);
    bodyBox = ICG_Frameless(335, 180, 940, 410);

    label1 = ICG_StaticPanel(335, 610, 170, 28, "");
    label2 = ICG_StaticPanel(520, 610, 170, 28, "");
    label3 = ICG_StaticPanel(705, 610, 170, 28, "");
    label4 = ICG_StaticPanel(890, 610, 235, 28, "");

    ICG_SetFontColor(label1, C_CYAN);
    ICG_SetFontColor(label2, C_CYAN);
    ICG_SetFontColor(label3, C_CYAN);
    ICG_SetFontColor(label4, C_CYAN);

    edit1 = ICG_SLEditSunken(335, 645, 170, 34, "");
    edit2 = ICG_SLEditSunken(520, 645, 170, 34, "");
    edit3 = ICG_SLEditSunken(705, 645, 170, 34, "");
    edit4 = ICG_SLEditSunken(890, 645, 235, 34, "");

    DrawRun(runImg);
    runButtonHandle = ICG_BitmapButton(1140, 641, 205, 40, RunAction);
    SetButtonBitmap(runButtonHandle, runImg);

    ShowDashboard();
    InstallClockTimer();
}