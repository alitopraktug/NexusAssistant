#include "app.h"

void SaveTasks()
{
    ofstream f("tasks.txt");

    for (Task& t : tasks)
        f << t.date << "|" << t.time << "|" << t.priority << "|" << t.title << "\n";
}

void LoadTasks()
{
    tasks.clear();

    ifstream f("tasks.txt");
    string line;

    while (getline(f, line))
    {
        vector<string> p = Split(line, '|');

        if (p.size() >= 4)
            tasks.push_back({ p[0], p[1], p[2], p[3] });
        else if (p.size() >= 3)
            tasks.push_back({ p[0], p[1], "normal", p[2] });
    }
}

void SaveSpecial()
{
    ofstream f("specialdays.txt");

    for (SpecialDay& s : specialDays)
        f << s.date << "|" << s.priority << "|" << s.name << "\n";
}

void LoadSpecial()
{
    specialDays.clear();

    ifstream f("specialdays.txt");
    string line;

    while (getline(f, line))
    {
        vector<string> p = Split(line, '|');

        if (p.size() >= 3)
            specialDays.push_back({ p[0], p[1], p[2] });
    }
}

void SaveSettings()
{
    ofstream f("settings.txt");

    f << "username=" << userName << "\n";
    f << "city=" << cityName << "\n";
    f << "timeformat=" << timeFormatMode << "\n";
    f << "weightunit=" << weightUnitMode << "\n";
}

void LoadSettings()
{
    ifstream f("settings.txt");
    string line;

    while (getline(f, line))
    {
        if (line.find("username=") == 0)
            userName = line.substr(9);

        if (line.find("city=") == 0)
            cityName = line.substr(5);

        if (line.find("timeformat=") == 0)
            timeFormatMode = line.substr(11);

        if (line.find("weightunit=") == 0)
            weightUnitMode = line.substr(11);
    }

    timeFormatMode = NormalizeTimeFormat(timeFormatMode);
    weightUnitMode = NormalizeWeightUnit(weightUnitMode);
}
void SaveLastUpdates()
{
    ofstream f("last_updates.txt");

    f << "weather=" << weatherLastUpdate << "\n";
    f << "sports=" << sportsLastUpdate << "\n";
}

void LoadLastUpdates()
{
    ifstream f("last_updates.txt");
    string line;

    while (getline(f, line))
    {
        if (line.find("weather=") == 0)
            weatherLastUpdate = line.substr(8);

        if (line.find("sports=") == 0)
            sportsLastUpdate = line.substr(7);
    }
}
void SaveFinance()
{
    ofstream f("finance.txt");

    f << incomeValue << "\n";
    f << expenseValue << "\n";
}

void LoadFinance()
{
    ifstream f("finance.txt");

    if (f.is_open())
        f >> incomeValue >> expenseValue;
}

void SaveFinanceLog()
{
    ofstream f("finance_log.txt");

    for (FinanceItem& item : financeLogs)
    {
        f << item.date << "|";
        f << item.type << "|";
        f << item.amount << "|";
        f << item.description << "\n";
    }
}

void LoadFinanceLog()
{
    financeLogs.clear();

    ifstream f("finance_log.txt");
    string line;

    while (getline(f, line))
    {
        vector<string> p = Split(line, '|');

        if (p.size() >= 4)
        {
            FinanceItem item;
            item.date = p[0];
            item.type = p[1];
            item.amount = atof(p[2].c_str());
            item.description = p[3];

            financeLogs.push_back(item);
        }
    }
}

void SavePortfolio()
{
    ofstream f("portfolio.txt");

    f << cashTL << "\n";
    f << usdAmount << "\n";
    f << euroAmount << "\n";
    f << goldGram << "\n";
}

void LoadPortfolio()
{
    ifstream f("portfolio.txt");

    if (f.is_open())
    {
        f >> cashTL;

        if (!(f >> usdAmount))
            usdAmount = 0;

        if (!(f >> euroAmount))
            euroAmount = 0;

        if (!(f >> goldGram))
            goldGram = 0;
    }

    if (cashTL < 0)
        cashTL = 0;

    if (usdAmount < 0)
        usdAmount = 0;

    if (euroAmount < 0)
        euroAmount = 0;

    if (goldGram < 0)
        goldGram = 0;
}

void SaveVehicle()
{
    ofstream f("vehicle.txt");

    f << currentKm << "\n";
    f << lastServiceKm << "\n";
    f << serviceIntervalKm << "\n";
    f << fuelTotalCost << "\n";
}

void LoadVehicle()
{
    ifstream f("vehicle.txt");

    if (f.is_open())
    {
        f >> currentKm;
        f >> lastServiceKm;

        if (!(f >> serviceIntervalKm))
            serviceIntervalKm = 10000;

        if (!(f >> fuelTotalCost))
            fuelTotalCost = 0;
    }

    if (serviceIntervalKm <= 0)
        serviceIntervalKm = 10000;

    if (fuelTotalCost < 0)
        fuelTotalCost = 0;
}

void SaveWorkout()
{
    ofstream f("workout.txt");

    f << weightKg << "\n";
    f << heightCm << "\n";
    f << workoutProgress << "\n";
    f << targetWeightKg << "\n";
    f << weeklyWorkoutGoal << "\n";
}

void LoadWorkout()
{
    ifstream f("workout.txt");

    if (f.is_open())
    {
        f >> weightKg;
        f >> heightCm;

        if (!(f >> workoutProgress))
            workoutProgress = 0;

        if (!(f >> targetWeightKg))
            targetWeightKg = weightKg;

        if (!(f >> weeklyWorkoutGoal))
            weeklyWorkoutGoal = 3;
    }

    if (weightKg <= 0)
        weightKg = 70;

    if (heightCm <= 0)
        heightCm = 175;

    if (targetWeightKg <= 0)
        targetWeightKg = weightKg;

    if (weeklyWorkoutGoal <= 0)
        weeklyWorkoutGoal = 3;

    if (weeklyWorkoutGoal > 7)
        weeklyWorkoutGoal = 7;
}

void LoadAll()
{
    LoadTasks();
    LoadSpecial();
    LoadSettings();
    LoadLastUpdates();
    LoadFinance();
    LoadFinanceLog();
    LoadPortfolio();
    LoadVehicle();
    LoadWorkout();
}

int TodayTaskCount()
{
    int c = 0;

    for (Task& t : tasks)
        if (t.date == Today())
            c++;

    return c;
}

int ImportantCount()
{
    int c = 0;

    for (Task& t : tasks)
        if (t.date == Today() && t.priority == "important")
            c++;

    for (SpecialDay& s : specialDays)
        if (DaysUntil(s.date) >= 0 && DaysUntil(s.date) <= 7 && s.priority == "important")
            c++;

    return c;
}

string NextTask()
{
    for (Task& t : tasks)
        if (t.date == Today())
            return t.time + " " + t.title;

    return "No event";
}

double Balance()
{
    return incomeValue - expenseValue;
}

long long RemainingService()
{
    if (serviceIntervalKm <= 0)
        serviceIntervalKm = 10000;

    long long next = lastServiceKm + serviceIntervalKm;

    return next - currentKm;
}

double BMI()
{
    if (heightCm <= 0 || weightKg <= 0)
        return 0;

    double m = heightCm / 100.0;

    return weightKg / (m * m);
}

string BMIStatus()
{
    double b = BMI();

    if (b <= 0)
        return "Not set";

    if (b < 18.5)
        return "Low";

    if (b < 25)
        return "Normal";

    if (b < 30)
        return "High";

    return "Very high";
}

int NotificationCount()
{
    int c = 0;

    for (Task& t : tasks)
        if (t.date == Today() && t.priority == "important")
            c++;

    for (SpecialDay& s : specialDays)
        if (DaysUntil(s.date) >= 0 && DaysUntil(s.date) <= 7 && s.priority == "important")
            c++;

    if (currentKm > 0 && lastServiceKm > 0 && RemainingService() <= 1000)
        c++;

    if (BMI() > 0 && (BMI() < 18.5 || BMI() >= 25))
        c++;

    return c;
}