#include "app.h"

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winhttp.lib")

int titleBox = 0;
int bodyBox = 0;
int label1 = 0;
int label2 = 0;
int label3 = 0;
int label4 = 0;
int edit1 = 0;
int edit2 = 0;
int edit3 = 0;
int edit4 = 0;
int runButtonHandle = 0;
int clockBoxHandle = 0;

bool uiReady = false;

ULONG_PTR gdiplusToken = 0;
HWND cardWnd[9] = {};
HBITMAP cardBmp[9] = {};
WNDPROC cardOldProc[9] = {};

ICBYTES titleImg;
ICBYTES bodyImg;

ICBYTES logoImg;
ICBYTES clockImg;
ICBYTES panelImg;
ICBYTES runImg;
ICBYTES menuImgs[MENU_COUNT];

int menuHandles[MENU_COUNT] = {};

extern const unsigned C_PANEL = 0x2A170F;
extern const unsigned C_DARK = 0x20120B;
extern const unsigned C_ACTIVE = 0x443012;
extern const unsigned C_BORDER = 0x5F3A1E;
extern const unsigned C_CYAN = 0xFFE500;
extern const unsigned C_WHITE = 0xFFFFFF;

Page currentPage = DASHBOARD;

vector<Task> tasks;
vector<SpecialDay> specialDays;
vector<FinanceItem> financeLogs;

string userName = "User";
string cityName = "Adana";
string timeFormatMode = "24";
string weightUnitMode = "kg";
string weatherLastUpdate = "";
string sportsLastUpdate = "";

double incomeValue = 0;
double expenseValue = 0;
double cashTL = 0;
double usdAmount = 0;
double euroAmount = 0;
double goldGram = 0;

long long currentKm = 0;
long long lastServiceKm = 0;
long long serviceIntervalKm = 10000;
double fuelTotalCost = 0;

double weightKg = 70;
double heightCm = 175;
int workoutProgress = 0;
double targetWeightKg = 70;
int weeklyWorkoutGoal = 3;

void ICGUI_Create()
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    ICG_MWTitle(TEXT("Nexus Assistant"));
    ICG_MWSize(1450, 820);
    ICG_MWPosition(50, 25);
    ICG_MWColor(8, 12, 20);
    ICG_SetFont(18, 0, "Consolas");
}

void ICGUI_main()
{
    if (!uiReady)
    {
        uiReady = true;
        BuildUI();
    }
}