#pragma once

#include "icb_gui.h"
#include "ic_media.h"
#include "icbytes.h"

#include <windows.h>
#include <winhttp.h>
#include <gdiplus.h>
#include <richedit.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cmath>

using namespace std;

constexpr int MENU_COUNT = 12;

extern int titleBox;
extern int bodyBox;
extern int label1;
extern int label2;
extern int label3;
extern int label4;
extern int edit1;
extern int edit2;
extern int edit3;
extern int edit4;
extern int runButtonHandle;
extern int clockBoxHandle;

extern bool uiReady;

extern ULONG_PTR gdiplusToken;
extern HWND cardWnd[9];
extern HBITMAP cardBmp[9];
extern WNDPROC cardOldProc[9];

extern ICBYTES titleImg;
extern ICBYTES bodyImg;

extern ICBYTES logoImg;
extern ICBYTES clockImg;
extern ICBYTES panelImg;
extern ICBYTES runImg;
extern ICBYTES menuImgs[MENU_COUNT];

extern int menuHandles[MENU_COUNT];

extern const unsigned C_PANEL;
extern const unsigned C_DARK;
extern const unsigned C_ACTIVE;
extern const unsigned C_BORDER;
extern const unsigned C_CYAN;
extern const unsigned C_WHITE;

enum Page {
    DASHBOARD,
    SCHEDULE,
    NOTIFICATIONS,
    SPECIAL,
    SPORTS,
    WEATHER,
    VEHICLE,
    FINANCE,
    PORTFOLIO,
    WORKOUT,
    AI,
    SETTINGS
};

extern Page currentPage;

struct Task {
    string date;
    string time;
    string priority;
    string title;
};

struct SpecialDay {
    string date;
    string priority;
    string name;
};

struct FinanceItem {
    string date;
    string type;
    double amount;
    string description;
};

extern vector<Task> tasks;
extern vector<SpecialDay> specialDays;
extern vector<FinanceItem> financeLogs;

extern string userName;
extern string cityName;
extern string timeFormatMode;
extern string weightUnitMode;
extern string weatherLastUpdate;
extern string sportsLastUpdate;

extern double incomeValue;
extern double expenseValue;
extern double cashTL;
extern double usdAmount;
extern double euroAmount;
extern double goldGram;

extern long long currentKm;
extern long long lastServiceKm;
extern long long serviceIntervalKm;
extern double fuelTotalCost;

extern double weightKg;
extern double heightCm;
extern int workoutProgress;
extern double targetWeightKg;
extern int weeklyWorkoutGoal;

extern const char* menuNames[MENU_COUNT];
extern void (*menuCallbacks[MENU_COUNT])();

string Trim(string s);
string Lower(string s);
string Today();
string NowText();
time_t DateToTime(string d);
int DaysUntil(string date);
string GetEdit(int h);
void SetEditText(int h, string s);
vector<string> Split(string s, char c);
string Priority(string s);
string ToStr(double x);
string NormalizeTimeFormat(string s);
string NormalizeWeightUnit(string s);
string WeightUnitLabel();
double WeightToDisplay(double kg);
double WeightInputToKg(double value);
string FormatWeight(double kg);

void LoadAll();
void SaveTasks();
void SaveSpecial();
void SaveSettings();
void SaveLastUpdates();
void SaveFinance();
void SaveFinanceLog();
void SavePortfolio();
void SaveVehicle();
void SaveWorkout();

int TodayTaskCount();
int ImportantCount();
int NotificationCount();
string NextTask();
double Balance();
long long RemainingService();
double BMI();
string BMIStatus();

string TasksText();
string AllScheduleText();
string SpecialText();
string NotificationsText();
string FinanceText();
string PortfolioText();
string VehicleText();
string WorkoutText();
string WeatherText();
string SportsText();
string FetchSportsText();
string FetchWeatherText(string city);
string HelpText();
string DashboardText();
string AIAnswer(string cmd);
string LastUpdateLine(string name, string value);

Gdiplus::Color GColor(unsigned rgb);
wstring W(string s);

void DrawTop(ICBYTES& img, const char* text, int w, unsigned color);
void DrawPanel(ICBYTES& img);
void DrawButton(ICBYTES& img, const char* text, bool active);
void DrawRun(ICBYTES& img);
void UpdateDashboardCards();
void ShowDashboardCards(bool show);
void ShowTextArea(bool show);

void RefreshMenu();
void SetPageText(const char* title, string text);
void SetInputs(string l1, string v1, string l2, string v2, string l3, string v3, string l4, string v4);
void ShowRunButton(bool show);
void SetRunText(string text);
void EnterPage(Page p, const char* title, string body);

void ShowDashboard();
void ShowSchedule();
void ShowNotifications();
void ShowSpecial();
void ShowSports();
void ShowWeather();
void ShowVehicle();
void ShowFinance();
void ShowPortfolio();
void ShowWorkout();
void ShowAI();
void ShowSettings();
void RunAction();
void RefreshTopClock();
void InstallClockTimer();

void BuildUI();