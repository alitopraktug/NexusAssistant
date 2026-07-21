#include "app.h"

string CleanText(string s)
{
    while (s.find("TÃ¼rkiye") != string::npos)
        s.replace(s.find("TÃ¼rkiye"), 9, "Turkiye");

    while (s.find("TÃ¼rk") != string::npos)
        s.replace(s.find("TÃ¼rk"), 5, "Turk");

    while (s.find("Ä±") != string::npos)
        s.replace(s.find("Ä±"), 2, "i");

    while (s.find("ÅŸ") != string::npos)
        s.replace(s.find("ÅŸ"), 2, "s");

    while (s.find("ÄŸ") != string::npos)
        s.replace(s.find("ÄŸ"), 2, "g");

    while (s.find("Ã§") != string::npos)
        s.replace(s.find("Ã§"), 2, "c");

    while (s.find("Ã¶") != string::npos)
        s.replace(s.find("Ã¶"), 2, "o");

    while (s.find("Ã¼") != string::npos)
        s.replace(s.find("Ã¼"), 2, "u");

    while (s.find("Å") != string::npos)
        s.replace(s.find("Å"), 1, "");

    return s;
}

string NormalizeTimeFormat(string s)
{
    s = Lower(Trim(s));

    if (
        s == "12" ||
        s == "12h" ||
        s == "am" ||
        s == "pm" ||
        s == "am/pm" ||
        s == "ampm"
        )
    {
        return "12";
    }

    return "24";
}

string NormalizeWeightUnit(string s)
{
    s = Lower(Trim(s));

    if (
        s == "lb" ||
        s == "lbs" ||
        s == "pound" ||
        s == "pounds"
        )
    {
        return "lb";
    }

    return "kg";
}

string WeightUnitLabel()
{
    weightUnitMode = NormalizeWeightUnit(weightUnitMode);

    return weightUnitMode;
}

double WeightToDisplay(double kg)
{
    if (WeightUnitLabel() == "lb")
        return kg * 2.20462;

    return kg;
}

double WeightInputToKg(double value)
{
    if (WeightUnitLabel() == "lb")
        return value / 2.20462;

    return value;
}

string FormatWeight(double kg)
{
    stringstream ss;

    ss << fixed << setprecision(1);
    ss << WeightToDisplay(kg) << " " << WeightUnitLabel();

    return ss.str();
}

string LastUpdateLine(string name, string value)
{
    if (value.empty())
        return "\n\nLast " + name + " Update: Not refreshed yet.";

    return "\n\nLast " + name + " Update: " + value;
}

string TasksText()
{
    string s = "Today's Schedule\n\n";
    int count = 0;

    for (Task& t : tasks)
    {
        if (t.date == Today())
        {
            s += t.time + "  [" + t.priority + "]  " + t.title + "\n";
            count++;
        }
    }

    if (count == 0)
        s += "No schedule item for today.\n";

    return s;
}

string AllScheduleText()
{
    string s = "Schedule Items\n\n";

    if (tasks.empty())
        s += "No schedule item saved.\n";

    for (int i = 0; i < (int)tasks.size(); i++)
    {
        Task& t = tasks[i];

        s += to_string(i + 1) + ") ";
        s += t.date + "  ";
        s += t.time + "  ";
        s += "[" + t.priority + "]  ";
        s += t.title + "\n";
    }

    s += "\nAdd Format:\n";
    s += "Date | Time | Priority | Task\n\n";
    s += "Delete Format:\n";
    s += "Write delete 1 in Task field and press Add Schedule.\n";
    s += "You can also write sil 1.";

    return s;
}

string SpecialText()
{
    string s = "Special Days\n\n";

    if (specialDays.empty())
        s += "No special day saved.\n";

    for (int i = 0; i < (int)specialDays.size(); i++)
    {
        SpecialDay& sp = specialDays[i];
        int d = DaysUntil(sp.date);

        s += to_string(i + 1) + ") ";
        s += sp.date + "  ";
        s += "[" + sp.priority + "]  ";
        s += sp.name;

        if (d >= 0 && d <= 7)
            s += "  -> soon";

        s += "\n";
    }

    s += "\nAdd Format:\n";
    s += "Date | Priority | Name\n\n";
    s += "Delete Format:\n";
    s += "Write delete 1 in Name field and press Add Special.\n";
    s += "You can also write sil 1.";

    return s;
}

string NotificationsText()
{
    string s = "Notifications\n\n";
    int count = 0;

    int todayCount = TodayTaskCount();

    if (todayCount > 0)
    {
        s += "- You have " + to_string(todayCount) + " schedule item(s) today.\n";
        count++;
    }

    for (Task& t : tasks)
    {
        if (t.date == Today() && t.priority == "important")
        {
            s += "- Important today: " + t.time + " " + t.title + "\n";
            count++;
        }
    }

    for (SpecialDay& sp : specialDays)
    {
        int d = DaysUntil(sp.date);

        if (d >= 0 && d <= 7)
        {
            s += "- Special day soon: " + sp.name + " in " + to_string(d) + " day(s)";

            if (sp.priority == "important")
                s += " [important]";

            s += "\n";
            count++;
        }
    }

    if (currentKm > 0 && lastServiceKm > 0)
    {
        long long r = RemainingService();

        if (r <= 0)
        {
            s += "- Vehicle service is due. Passed by " + to_string(-r) + " km.\n";
            count++;
        }
        else if (r <= 1000)
        {
            s += "- Vehicle service is close: " + to_string(r) + " km remaining.\n";
            count++;
        }
    }

    double b = BMI();

    if (b > 0 && b < 18.5)
    {
        s += "- BMI alert: below normal range.\n";
        count++;
    }

    if (b >= 25 && b < 30)
    {
        s += "- BMI alert: above normal range.\n";
        count++;
    }

    if (b >= 30)
    {
        s += "- BMI alert: very high range.\n";
        count++;
    }

    if (Balance() < 0)
    {
        s += "- Finance alert: expenses are higher than income.\n";
        count++;
    }

    if (weatherLastUpdate.empty())
    {
        s += "- Weather has not been refreshed yet.\n";
        count++;
    }

    if (sportsLastUpdate.empty())
    {
        s += "- Sports data has not been refreshed yet.\n";
        count++;
    }

    if (count == 0)
    {
        s += "No important notification right now.\n";
    }

    s += "\nSummary\n";
    s += "Total alerts: " + to_string(count) + "\n";

    return s;
}

string FinanceLogText()
{
    string s = "Transaction History\n\n";

    if (financeLogs.empty())
    {
        s += "No finance transaction saved.\n";
        return s;
    }

    for (int i = 0; i < (int)financeLogs.size(); i++)
    {
        FinanceItem& item = financeLogs[i];

        string sign = "+";

        if (item.type == "expense")
            sign = "-";

        s += to_string(i + 1) + ") ";
        s += item.date + "  ";
        s += sign + ToStr(item.amount) + " TL  ";
        s += "[" + item.type + "]  ";
        s += item.description + "\n";
    }

    return s;
}

string FinanceText()
{
    stringstream ss;
    ss << fixed << setprecision(2);

    ss << "Finance Dashboard\n\n";
    ss << "Total Income: " << incomeValue << " TL\n";
    ss << "Total Expense: " << expenseValue << " TL\n";
    ss << "Balance: " << Balance() << " TL\n\n";

    ss << FinanceLogText();

    ss << "\nCommands\n";
    ss << "Description: Salary\n";
    ss << "Amount: 500\n";
    ss << "Type: income or expense\n\n";
    ss << "Delete: write delete 1 in Description.\n";
    ss << "Clear all: write clear in Description.";

    return ss.str();
}

string PortfolioText()
{
    double usdRate = 35.0;
    double euroRate = 38.0;
    double goldRate = 2450.0;

    double usdValue = usdAmount * usdRate;
    double euroValue = euroAmount * euroRate;
    double goldValue = goldGram * goldRate;

    double total = cashTL + usdValue + euroValue + goldValue;

    stringstream ss;
    ss << fixed << setprecision(2);

    ss << "Portfolio Overview\n\n";

    ss << "Assets\n";
    ss << "Cash TL: " << cashTL << " TL\n";
    ss << "USD: " << usdAmount << " -> " << usdValue << " TL\n";
    ss << "EUR: " << euroAmount << " -> " << euroValue << " TL\n";
    ss << "Gold: " << goldGram << " gram -> " << goldValue << " TL\n\n";

    ss << "Estimated Rates\n";
    ss << "USD: " << usdRate << " TL\n";
    ss << "EUR: " << euroRate << " TL\n";
    ss << "Gold Gram: " << goldRate << " TL\n\n";

    ss << "Estimated Total Value\n";
    ss << total << " TL\n\n";

    ss << "Allocation\n";

    if (total > 0)
    {
        ss << "Cash: " << (cashTL / total) * 100.0 << "%\n";
        ss << "USD: " << (usdValue / total) * 100.0 << "%\n";
        ss << "EUR: " << (euroValue / total) * 100.0 << "%\n";
        ss << "Gold: " << (goldValue / total) * 100.0 << "%\n";
    }
    else
    {
        ss << "No asset saved yet.\n";
    }

    ss << "\nNexus says:\n";

    if (total <= 0)
        ss << "Enter assets below to calculate your portfolio.";
    else if (cashTL / total > 0.70)
        ss << "Most of your portfolio is cash.";
    else if (goldValue / total > 0.50)
        ss << "Gold has a strong weight in your portfolio.";
    else
        ss << "Your portfolio looks diversified.";

    return ss.str();
}

string VehicleText()
{
    if (serviceIntervalKm <= 0)
        serviceIntervalKm = 10000;

    long long nextService = lastServiceKm + serviceIntervalKm;
    long long remaining = nextService - currentKm;
    long long drivenAfterService = currentKm - lastServiceKm;

    if (drivenAfterService < 0)
        drivenAfterService = 0;

    double costPerKm = 0;
    double costPer100Km = 0;

    if (fuelTotalCost > 0 && drivenAfterService > 0)
    {
        costPerKm = fuelTotalCost / drivenAfterService;
        costPer100Km = costPerKm * 100.0;
    }

    stringstream ss;
    ss << fixed << setprecision(2);

    ss << "Vehicle Status\n\n";
    ss << "Current KM: " << currentKm << "\n";
    ss << "Last Service KM: " << lastServiceKm << "\n";
    ss << "Service Interval: " << serviceIntervalKm << " km\n";
    ss << "Next Service KM: " << nextService << "\n";
    ss << "Remaining Service: " << remaining << " km\n";
    ss << "Driven After Service: " << drivenAfterService << " km\n\n";

    ss << "Fuel Analysis\n";
    ss << "Fuel Total: " << fuelTotalCost << " TL\n";

    if (fuelTotalCost > 0 && drivenAfterService > 0)
    {
        ss << "Cost / KM: " << costPerKm << " TL\n";
        ss << "Cost / 100 KM: " << costPer100Km << " TL\n";
    }
    else
    {
        ss << "Cost / KM: not enough data\n";
    }

    ss << "\nNexus says:\n";

    if (currentKm == 0 || lastServiceKm == 0)
        ss << "Enter vehicle data first.";
    else if (remaining > 1000)
        ss << "Service status is good.";
    else if (remaining > 0)
        ss << "Service is close.";
    else
        ss << "Service time has arrived.";

    return ss.str();
}

string WorkoutText()
{
    stringstream ss;
    ss << fixed << setprecision(1);

    double b = BMI();

    double diffKg = weightKg - targetWeightKg;

    if (diffKg < 0)
        diffKg = -diffKg;

    double diffDisplay = WeightToDisplay(diffKg);

    ss << "Workout & Body Goal\n\n";

    ss << "Current Weight: " << FormatWeight(weightKg) << "\n";
    ss << "Height: " << heightCm << " cm\n";
    ss << "Target Weight: " << FormatWeight(targetWeightKg) << "\n";
    ss << "Weekly Workout Goal: " << weeklyWorkoutGoal << " day(s)\n";
    ss << "Weight Unit: " << WeightUnitLabel() << "\n\n";

    ss << "BMI Analysis\n";
    ss << "BMI: " << b << "\n";
    ss << "Status: " << BMIStatus() << "\n\n";

    ss << "Target Analysis\n";

    if (diffKg < 0.3)
    {
        ss << "You are already very close to your target weight.\n";
    }
    else if (weightKg > targetWeightKg)
    {
        ss << "You need to lose about " << diffDisplay << " " << WeightUnitLabel() << ".\n";
        ss << "Focus: calorie control + walking + strength training.\n";
    }
    else
    {
        ss << "You need to gain about " << diffDisplay << " " << WeightUnitLabel() << ".\n";
        ss << "Focus: healthy calorie surplus + strength training.\n";
    }

    ss << "\nSuggested Weekly Plan\n";

    if (weeklyWorkoutGoal <= 2)
    {
        ss << "2 full-body workouts + daily light walking.\n";
    }
    else if (weeklyWorkoutGoal <= 4)
    {
        ss << "3 strength days + 1 cardio/walk day.\n";
    }
    else
    {
        ss << "4 strength days + 2 cardio days + 1 rest day.\n";
    }

    ss << "\nNexus Coach:\n";

    if (b < 18.5)
        ss << "Gain weight slowly with quality food.";
    else if (b < 25)
        ss << "Your BMI is normal. Focus on consistency.";
    else if (b < 30)
        ss << "Small calorie deficit and walking can help.";
    else
        ss << "Start gently and increase activity gradually.";

    return ss.str();
}

string WeatherText()
{
    ifstream f("weather_cache.txt");
    stringstream ss;
    ss << f.rdbuf();

    string cache = CleanText(ss.str());

    if (!cache.empty())
    {
        string out = cache;

        out += LastUpdateLine("Weather", weatherLastUpdate);
        out += "\n\nRefresh to update current weather.";

        return out;
    }

    stringstream out;

    out << cityName << " Weather\n\n";
    out << "No weather data yet.\n";
    out << "Enter city and press Refresh Weather.\n\n";

    if (weatherLastUpdate.empty())
        out << "Last Weather Update: Not refreshed yet.";
    else
        out << "Last Weather Update: " << weatherLastUpdate;

    return out.str();
}

string HelpText()
{
    return
        "Nexus AI Assistant\n\n"
        "Question Commands\n\n"
        "bugun ne var\n"
        "onemli ne var\n"
        "bildirimler\n"
        "ozel gunler\n"
        "spor maclari\n"
        "bmi hesapla\n"
        "arac durumum\n"
        "param ne durumda\n"
        "hava nasil\n\n"

        "Action Commands\n\n"
        "bugun 09:00 ders ekle\n"
        "yarin 14:30 toplanti ekle\n"
        "onemli bugun 10:00 sinav ekle\n"
        "gorev 1 sil\n"
        "gelir 500 harclik ekle\n"
        "gider 120 yemek ekle\n"
        "hava yenile\n"
        "spor yenile\n"
        "backup al\n"
        "yedek al\n"
        "export\n\n"

        "Type a command below and press Ask Nexus.";
}

string DashboardText()
{
    stringstream ss;

    ss << "Dashboard\n\n";
    ss << "Today: " << TodayTaskCount() << " item(s)\n";
    ss << "Important: " << ImportantCount() << "\n";
    ss << "Notifications: " << NotificationCount() << "\n";
    ss << "Balance: " << fixed << setprecision(2) << Balance() << " TL\n";
    ss << "BMI: " << fixed << setprecision(1) << BMI() << " (" << BMIStatus() << ")\n";

    return ss.str();
}

int DeleteIndexFromText(string s)
{
    s = Lower(Trim(s));

    if (s.find("delete ") == 0)
        return atoi(s.substr(7).c_str());

    if (s.find("sil ") == 0)
        return atoi(s.substr(4).c_str());

    return 0;
}

int FirstNumberInt(string s)
{
    string num = "";

    for (int i = 0; i < (int)s.size(); i++)
    {
        if (s[i] >= '0' && s[i] <= '9')
        {
            while (i < (int)s.size() && s[i] >= '0' && s[i] <= '9')
            {
                num += s[i];
                i++;
            }

            break;
        }
    }

    if (num.empty())
        return 0;

    return atoi(num.c_str());
}

double FirstNumberDouble(string s)
{
    string num = "";

    for (int i = 0; i < (int)s.size(); i++)
    {
        if ((s[i] >= '0' && s[i] <= '9') || s[i] == ',' || s[i] == '.')
        {
            while (
                i < (int)s.size() &&
                (
                    (s[i] >= '0' && s[i] <= '9') ||
                    s[i] == ',' ||
                    s[i] == '.'
                    )
                )
            {
                if (s[i] == ',')
                    num += '.';
                else
                    num += s[i];

                i++;
            }

            break;
        }
    }

    if (num.empty())
        return 0;

    return atof(num.c_str());
}

string TomorrowDate()
{
    time_t now = time(NULL);
    now += 24 * 60 * 60;

    tm t;
    localtime_s(&t, &now);

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &t);

    return string(buf);
}

string FindTimeToken(string s)
{
    stringstream ss(s);
    string token;

    while (ss >> token)
    {
        if (token.find(":") != string::npos)
            return token;
    }

    return "09:00";
}

string RemoveTrailingCommandWords(string s)
{
    s = Trim(s);

    string low = Lower(s);

    if (low.size() >= 4 && low.substr(low.size() - 4) == "ekle")
        s = Trim(s.substr(0, s.size() - 4));

    low = Lower(s);

    if (low.size() >= 3 && low.substr(low.size() - 3) == "add")
        s = Trim(s.substr(0, s.size() - 3));

    low = Lower(s);

    if (low.size() >= 4 && low.substr(low.size() - 4) == "task")
        s = Trim(s.substr(0, s.size() - 4));

    low = Lower(s);

    if (low.size() >= 5 && low.substr(low.size() - 5) == "gorev")
        s = Trim(s.substr(0, s.size() - 5));

    return s;
}

string TextAfterTime(string cmd, string timeToken)
{
    string low = Lower(cmd);

    size_t pos = low.find(timeToken);

    if (pos == string::npos)
        return "AI Task";

    string title = cmd.substr(pos + timeToken.size());
    title = RemoveTrailingCommandWords(title);
    title = Trim(title);

    if (title.empty())
        title = "AI Task";

    return title;
}

string TextAfterFirstNumber(string cmd)
{
    int start = -1;
    int end = -1;

    for (int i = 0; i < (int)cmd.size(); i++)
    {
        if ((cmd[i] >= '0' && cmd[i] <= '9') || cmd[i] == ',' || cmd[i] == '.')
        {
            start = i;

            while (
                i < (int)cmd.size() &&
                (
                    (cmd[i] >= '0' && cmd[i] <= '9') ||
                    cmd[i] == ',' ||
                    cmd[i] == '.'
                    )
                )
            {
                i++;
            }

            end = i;
            break;
        }
    }

    if (start == -1 || end == -1)
        return "";

    string text = cmd.substr(end);
    text = RemoveTrailingCommandWords(text);
    text = Trim(text);

    return text;
}

string AIAddTask(string cmd)
{
    string low = Lower(cmd);

    string date = Today();

    if (low.find("yarin") != string::npos || low.find("tomorrow") != string::npos)
        date = TomorrowDate();

    string timeValue = FindTimeToken(cmd);
    string title = TextAfterTime(cmd, timeValue);

    string pr = "normal";

    if (low.find("onemli") != string::npos || low.find("important") != string::npos)
        pr = "important";

    tasks.push_back({ date, timeValue, pr, title });

    SaveTasks();

    string result = "Nexus Action Result\n\n";
    result += "Task added successfully.\n\n";
    result += "Date: " + date + "\n";
    result += "Time: " + timeValue + "\n";
    result += "Priority: " + pr + "\n";
    result += "Task: " + title + "\n\n";
    result += AllScheduleText();

    return result;
}

string AIDeleteTask(string cmd)
{
    int id = FirstNumberInt(cmd);

    if (id <= 0)
        return "Nexus Action Result\n\nTask ID is required.\nExample: gorev 1 sil";

    if (id > (int)tasks.size())
        return "Nexus Action Result\n\nInvalid task ID.\n\n" + AllScheduleText();

    Task deleted = tasks[id - 1];

    tasks.erase(tasks.begin() + id - 1);

    SaveTasks();

    string result = "Nexus Action Result\n\n";
    result += "Task deleted successfully.\n\n";
    result += "Deleted: " + deleted.date + " " + deleted.time + " " + deleted.title + "\n\n";
    result += AllScheduleText();

    return result;
}

string AIAddFinance(string cmd)
{
    string low = Lower(cmd);

    double amount = FirstNumberDouble(cmd);

    if (amount <= 0)
        return "Nexus Action Result\n\nAmount is required.\nExample: gelir 500 harclik ekle";

    string desc = TextAfterFirstNumber(cmd);

    if (desc.empty())
        desc = "AI Finance Item";

    FinanceItem item;
    item.date = NowText();
    item.amount = amount;
    item.description = desc;

    if (low.find("gider") != string::npos || low.find("expense") != string::npos || low.find("harcama") != string::npos)
    {
        item.type = "expense";
        expenseValue += amount;
    }
    else
    {
        item.type = "income";
        incomeValue += amount;
    }

    financeLogs.push_back(item);

    SaveFinance();
    SaveFinanceLog();

    string result = "Nexus Action Result\n\n";
    result += "Finance item added successfully.\n\n";
    result += "Type: " + item.type + "\n";
    result += "Amount: " + ToStr(item.amount) + " TL\n";
    result += "Description: " + item.description + "\n\n";
    result += FinanceText();

    return result;
}

string AIRefreshWeather()
{
    string result = "Nexus Action Result\n\n";
    result += "Weather refreshed.\n\n";

    string text = FetchWeatherText(cityName);
    text = CleanText(text);

    weatherLastUpdate = NowText();
    SaveLastUpdates();

    result += text;
    result += LastUpdateLine("Weather", weatherLastUpdate);

    return result;
}

string AIRefreshSports()
{
    string result = "Nexus Action Result\n\n";
    result += "Sports refreshed.\n\n";

    string text = FetchSportsText();

    sportsLastUpdate = NowText();
    SaveLastUpdates();

    result += text;
    result += LastUpdateLine("Sports", sportsLastUpdate);

    return result;
}

string CreateBackupFile()
{
    ofstream f("nexus_backup.txt");

    if (!f.is_open())
        return "Nexus Backup\n\nBackup file could not be created.";

    f << "NEXUS ASSISTANT BACKUP\n";
    f << "Created At: " << NowText() << "\n";
    f << "========================================\n\n";

    f << "[SETTINGS]\n";
    f << "User Name: " << userName << "\n";
    f << "City: " << cityName << "\n";
    f << "Time Format: " << timeFormatMode << "\n";
    f << "Weight Unit: " << weightUnitMode << "\n";
    f << "Weather Last Update: " << weatherLastUpdate << "\n";
    f << "Sports Last Update: " << sportsLastUpdate << "\n\n";

    f << "[SCHEDULE TASKS]\n";

    if (tasks.empty())
    {
        f << "No task saved.\n";
    }
    else
    {
        for (int i = 0; i < (int)tasks.size(); i++)
        {
            Task& t = tasks[i];

            f << i + 1 << ") ";
            f << t.date << " | ";
            f << t.time << " | ";
            f << t.priority << " | ";
            f << t.title << "\n";
        }
    }

    f << "\n[SPECIAL DAYS]\n";

    if (specialDays.empty())
    {
        f << "No special day saved.\n";
    }
    else
    {
        for (int i = 0; i < (int)specialDays.size(); i++)
        {
            SpecialDay& s = specialDays[i];

            f << i + 1 << ") ";
            f << s.date << " | ";
            f << s.priority << " | ";
            f << s.name << "\n";
        }
    }

    f << "\n[FINANCE]\n";
    f << fixed << setprecision(2);
    f << "Total Income: " << incomeValue << " TL\n";
    f << "Total Expense: " << expenseValue << " TL\n";
    f << "Balance: " << Balance() << " TL\n\n";

    f << "[FINANCE TRANSACTIONS]\n";

    if (financeLogs.empty())
    {
        f << "No finance transaction saved.\n";
    }
    else
    {
        for (int i = 0; i < (int)financeLogs.size(); i++)
        {
            FinanceItem& item = financeLogs[i];

            f << i + 1 << ") ";
            f << item.date << " | ";
            f << item.type << " | ";
            f << item.amount << " TL | ";
            f << item.description << "\n";
        }
    }

    f << "\n[PORTFOLIO]\n";
    f << "Cash TL: " << cashTL << "\n";
    f << "USD Amount: " << usdAmount << "\n";
    f << "EUR Amount: " << euroAmount << "\n";
    f << "Gold Gram: " << goldGram << "\n";

    f << "\n[VEHICLE]\n";
    f << "Current KM: " << currentKm << "\n";
    f << "Last Service KM: " << lastServiceKm << "\n";
    f << "Service Interval KM: " << serviceIntervalKm << "\n";
    f << "Fuel Total Cost: " << fuelTotalCost << " TL\n";
    f << "Remaining Service KM: " << RemainingService() << "\n";

    f << "\n[WORKOUT]\n";
    f << "Weight KG: " << weightKg << "\n";
    f << "Height CM: " << heightCm << "\n";
    f << "Target Weight KG: " << targetWeightKg << "\n";
    f << "Weekly Workout Goal: " << weeklyWorkoutGoal << "\n";
    f << "BMI: " << BMI() << "\n";
    f << "BMI Status: " << BMIStatus() << "\n";

    f << "\n========================================\n";
    f << "END OF BACKUP\n";

    f.close();

    string result = "Nexus Backup\n\n";
    result += "Backup created successfully.\n\n";
    result += "File name: nexus_backup.txt\n";
    result += "Created At: " + NowText() + "\n\n";
    result += "Included: settings, tasks, special days, finance, portfolio, vehicle, workout.";

    return result;
}

string AIAnswer(string cmd)
{
    string low = Lower(Trim(cmd));

    if (low.empty())
        return HelpText();

    bool wantsAdd = low.find("ekle") != string::npos || low.find("add") != string::npos;
    bool wantsDelete = low.find("sil") != string::npos || low.find("delete") != string::npos;
    bool wantsRefresh =
        low.find("yenile") != string::npos ||
        low.find("refresh") != string::npos ||
        low.find("update") != string::npos;

    bool wantsBackup =
        low.find("backup") != string::npos ||
        low.find("yedek") != string::npos ||
        low.find("export") != string::npos ||
        low.find("disa aktar") != string::npos;

    if (wantsBackup)
        return CreateBackupFile();

    if (
        wantsDelete &&
        (
            low.find("gorev") != string::npos ||
            low.find("task") != string::npos
            )
        )
    {
        return AIDeleteTask(cmd);
    }

    if (
        wantsAdd &&
        (
            low.find("bugun") != string::npos ||
            low.find("yarin") != string::npos ||
            low.find("today") != string::npos ||
            low.find("tomorrow") != string::npos ||
            low.find(":") != string::npos
            )
        )
    {
        return AIAddTask(cmd);
    }

    if (
        wantsAdd &&
        (
            low.find("gelir") != string::npos ||
            low.find("gider") != string::npos ||
            low.find("income") != string::npos ||
            low.find("expense") != string::npos ||
            low.find("harcama") != string::npos
            )
        )
    {
        return AIAddFinance(cmd);
    }

    if (
        wantsRefresh &&
        (
            low.find("hava") != string::npos ||
            low.find("weather") != string::npos
            )
        )
    {
        return AIRefreshWeather();
    }

    if (
        wantsRefresh &&
        (
            low.find("spor") != string::npos ||
            low.find("sports") != string::npos ||
            low.find("mac") != string::npos
            )
        )
    {
        return AIRefreshSports();
    }

    if (low.find("bugun") != string::npos || low.find("today") != string::npos)
        return TasksText();

    if (low.find("onem") != string::npos || low.find("important") != string::npos)
        return "Important Items\n\n" + NotificationsText();

    if (low.find("bildirim") != string::npos || low.find("notification") != string::npos)
        return NotificationsText();

    if (low.find("ozel") != string::npos || low.find("special") != string::npos)
        return SpecialText();

    if (low.find("mac") != string::npos || low.find("sports") != string::npos || low.find("spor") != string::npos)
        return SportsText() + LastUpdateLine("Sports", sportsLastUpdate);

    if (low.find("bmi") != string::npos || low.find("workout") != string::npos || low.find("spor oner") != string::npos)
        return WorkoutText();

    if (low.find("arac") != string::npos || low.find("vehicle") != string::npos)
        return VehicleText();

    if (low.find("para") != string::npos || low.find("finance") != string::npos || low.find("param") != string::npos)
        return FinanceText();

    if (low.find("hava") != string::npos || low.find("weather") != string::npos)
        return WeatherText();

    return HelpText();
}

void RefreshMenu()
{
    for (int i = 0; i < MENU_COUNT; i++)
    {
        DrawButton(menuImgs[i], menuNames[i], i == currentPage);

        if (menuHandles[i])
            SetButtonBitmap(menuHandles[i], menuImgs[i]);
    }
}

void ShowTextArea(bool show)
{
    HWND hTitle = ICG_GetHWND(titleBox);
    HWND hBody = ICG_GetHWND(bodyBox);

    if (hTitle)
        ShowWindow(hTitle, show ? SW_SHOW : SW_HIDE);

    if (hBody)
        ShowWindow(hBody, show ? SW_SHOW : SW_HIDE);
}

vector<string> WrapLine(string line, int maxLen)
{
    vector<string> out;

    while ((int)line.size() > maxLen)
    {
        int cut = maxLen;

        for (int i = maxLen; i >= 0; i--)
        {
            if (line[i] == ' ')
            {
                cut = i;
                break;
            }
        }

        out.push_back(line.substr(0, cut));
        line = Trim(line.substr(cut));
    }

    out.push_back(line);

    return out;
}

vector<string> SplitAndWrapText(string text, int maxLen)
{
    vector<string> lines;
    stringstream ss(text);
    string line;

    while (getline(ss, line))
    {
        vector<string> wrapped = WrapLine(line, maxLen);

        for (string& w : wrapped)
            lines.push_back(w);
    }

    return lines;
}

void DrawTitleImage(const char* title)
{
    CreateImage(titleImg, 940, 38, ICB_UINT);

    FillRoundRect(titleImg, 0, 0, 940, 38, 0, 0x20120B);
    Line(titleImg, 0, 37, 940, 37, 0x5F3A1E);

    DrawTextWIN(titleImg, 12, 8, title, C_CYAN, 18, 0, "Consolas");
}

void DrawBodyImage(string text)
{
    text = CleanText(text);

    CreateImage(bodyImg, 940, 410, ICB_UINT);

    FillRoundRect(bodyImg, 0, 0, 940, 410, 0, 0x080C14);
    Line(bodyImg, 0, 409, 940, 409, 0x5F3A1E);
    Line(bodyImg, 939, 0, 939, 409, 0x5F3A1E);

    vector<string> lines = SplitAndWrapText(text, 92);

    int y = 12;
    int lineHeight = 18;

    for (int i = 0; i < (int)lines.size(); i++)
    {
        if (y > 382)
        {
            DrawTextWIN(bodyImg, 12, y, "...", C_CYAN, 16, 0, "Consolas");
            break;
        }

        unsigned color = C_WHITE;

        if (lines[i].find("Nexus") != string::npos)
            color = C_CYAN;

        if (lines[i].find("Dashboard") != string::npos)
            color = C_CYAN;

        if (lines[i].find("Summary") != string::npos)
            color = C_CYAN;

        DrawTextWIN(bodyImg, 12, y, lines[i].c_str(), color, 16, 0, "Consolas");

        y += lineHeight;
    }
}

void SetPageText(const char* title, string text)
{
    HWND hTitle = ICG_GetHWND(titleBox);
    HWND hBody = ICG_GetHWND(bodyBox);

    if (hTitle)
    {
        ShowWindow(hTitle, SW_SHOW);

        SetWindowPos(
            hTitle,
            HWND_TOP,
            335,
            130,
            940,
            38,
            SWP_SHOWWINDOW
        );

        DrawTitleImage(title);
        DisplayImage(titleBox, titleImg);

        InvalidateRect(hTitle, NULL, TRUE);
        UpdateWindow(hTitle);
    }

    if (hBody)
    {
        ShowWindow(hBody, SW_SHOW);

        SetWindowPos(
            hBody,
            HWND_TOP,
            335,
            180,
            940,
            410,
            SWP_SHOWWINDOW
        );

        DrawBodyImage(text);
        DisplayImage(bodyBox, bodyImg);

        InvalidateRect(hBody, NULL, TRUE);
        UpdateWindow(hBody);
    }
}

void EnterPage(Page p, const char* title, string body)
{
    currentPage = p;
    RefreshMenu();

    if (p == DASHBOARD)
    {
        UpdateDashboardCards();
        ShowDashboardCards(true);
        ShowTextArea(false);
        return;
    }

    ShowDashboardCards(false);
    ShowTextArea(true);

    SetPageText(title, body);
}

void ShowDashboard()
{
    EnterPage(DASHBOARD, "DASHBOARD", "");

    SetInputs("", "", "", "", "", "", "", "");
    ShowRunButton(false);

    ShowTextArea(false);
    ShowDashboardCards(true);
}

void ShowSchedule()
{
    EnterPage(SCHEDULE, "SCHEDULE", AllScheduleText());
    SetInputs("Date", Today(), "Time", "09:00", "Priority", "normal", "Task", "");
    SetRunText("Add Schedule");
}

void ShowNotifications()
{
    EnterPage(NOTIFICATIONS, "NOTIFICATIONS", NotificationsText());

    SetInputs(
        "Action",
        "refresh",
        "",
        "",
        "",
        "",
        "",
        ""
    );

    SetRunText("Refresh Alerts");
}

void ShowSpecial()
{
    EnterPage(SPECIAL, "SPECIAL DAYS", SpecialText());
    SetInputs("Date", Today(), "Priority", "important", "Name", "", "", "");
    SetRunText("Add Special");
}

string SportsPageTextFromData(string data)
{
    data = CleanText(data);

    if (Trim(data).empty())
        data = "No sports data available. Press Refresh.";

    string body;

    body += "Sports Center\n\n";

    body += "Featured Match\n";
    body += "Match: Turkiye vs Paraguay\n";
    body += "Competition: World Cup 2026\n";
    body += "Dashboard Card: featured match\n\n";

    body += "Live / Cached Match Data\n";
    body += data + "\n\n";

    body += "Update Status\n";

    if (sportsLastUpdate.empty())
        body += "Last Sports Update: Not refreshed yet.\n";
    else
        body += "Last Sports Update: " + sportsLastUpdate + "\n";

    body += "\nAction\n";
    body += "Press Refresh to update sports data.";

    return body;
}

string SportsPageText()
{
    return SportsPageTextFromData(SportsText());
}

void ShowSports()
{
    EnterPage(SPORTS, "SPORTS", SportsPageText());

    SetInputs("", "", "", "", "", "", "", "");
    ShowRunButton(true);
    SetRunText("Refresh");
}

void ShowWeather()
{
    EnterPage(WEATHER, "WEATHER", WeatherText());
    SetInputs("City", cityName, "", "", "", "", "", "");
    SetRunText("Refresh Weather");
}

void ShowVehicle()
{
    EnterPage(VEHICLE, "VEHICLE", VehicleText());

    SetInputs(
        "Current KM",
        to_string(currentKm),
        "Last Service",
        to_string(lastServiceKm),
        "Interval KM",
        to_string(serviceIntervalKm),
        "Fuel TL",
        ToStr(fuelTotalCost)
    );

    SetRunText("Calculate");
}

void ShowFinance()
{
    EnterPage(FINANCE, "FINANCE", FinanceText());
    SetInputs("Description", "", "Amount", "0", "Type", "income", "", "");
    SetRunText("Add Finance");
}

void ShowPortfolio()
{
    EnterPage(PORTFOLIO, "PORTFOLIO", PortfolioText());

    SetInputs(
        "Cash TL",
        ToStr(cashTL),
        "USD",
        ToStr(usdAmount),
        "EUR",
        ToStr(euroAmount),
        "Gold gr",
        ToStr(goldGram)
    );

    SetRunText("Save Portfolio");
}

void ShowWorkout()
{
    EnterPage(WORKOUT, "WORKOUT", WorkoutText());

    SetInputs(
        "Weight " + WeightUnitLabel(),
        ToStr(WeightToDisplay(weightKg)),
        "Height cm",
        ToStr(heightCm),
        "Target " + WeightUnitLabel(),
        ToStr(WeightToDisplay(targetWeightKg)),
        "Weekly Goal",
        to_string(weeklyWorkoutGoal)
    );

    SetRunText("Save Workout");
}

void ShowAI()
{
    EnterPage(AI, "AI ASSISTANT", HelpText());
    SetInputs("Command", "", "", "", "", "", "", "");
    SetRunText("Ask Nexus");
}

void ShowSettings()
{
    timeFormatMode = NormalizeTimeFormat(timeFormatMode);
    weightUnitMode = NormalizeWeightUnit(weightUnitMode);

    string body =
        "Settings\n\n"
        "User Name: " + userName + "\n"
        "City: " + cityName + "\n"
        "Theme: Classic Dark Brown\n"
        "Currency: TL\n"
        "Assistant Name: Nexus\n"
        "Time Format: " + timeFormatMode + "\n"
        "Weight Unit: " + weightUnitMode + "\n\n"

        "Time Format Options:\n"
        "24  -> normal time, example 17:45\n"
        "12  -> AM/PM time, example 05:45 PM\n\n"

        "Weight Unit Options:\n"
        "kg  -> kilogram\n"
        "lb  -> pounds\n\n"

        "Nexus says:\n"
        "Change settings below and press Save Settings.";

    EnterPage(SETTINGS, "SETTINGS", body);

    SetInputs(
        "User Name",
        userName,
        "City",
        cityName,
        "Time Format",
        timeFormatMode,
        "Weight Unit",
        weightUnitMode
    );

    SetRunText("Save Settings");
}

void RunAction()
{
    string a = Trim(GetEdit(edit1));
    string b = Trim(GetEdit(edit2));
    string c = Trim(GetEdit(edit3));
    string d = Trim(GetEdit(edit4));

    if (currentPage == SCHEDULE)
    {
        int deleteId = DeleteIndexFromText(d);

        if (deleteId > 0)
        {
            if (deleteId <= (int)tasks.size())
            {
                tasks.erase(tasks.begin() + deleteId - 1);
                SaveTasks();
                ShowSchedule();
                return;
            }

            SetPageText("SCHEDULE", AllScheduleText() + "\n\nNexus says:\nInvalid schedule ID.");
            return;
        }

        if (a.empty() || b.empty() || d.empty())
        {
            SetPageText("SCHEDULE", AllScheduleText() + "\n\nNexus says:\nDate, time and task are required.");
            return;
        }

        tasks.push_back({ a, b, Priority(c), d });
        SaveTasks();
        ShowSchedule();
        return;
    }

    if (currentPage == SPECIAL)
    {
        int deleteId = DeleteIndexFromText(c);

        if (deleteId > 0)
        {
            if (deleteId <= (int)specialDays.size())
            {
                specialDays.erase(specialDays.begin() + deleteId - 1);
                SaveSpecial();
                ShowSpecial();
                return;
            }

            SetPageText("SPECIAL DAYS", SpecialText() + "\n\nNexus says:\nInvalid special day ID.");
            return;
        }

        if (a.empty() || c.empty())
        {
            SetPageText("SPECIAL DAYS", SpecialText() + "\n\nNexus says:\nDate and name are required.");
            return;
        }

        specialDays.push_back({ a, Priority(b), c });
        SaveSpecial();
        ShowSpecial();
        return;
    }

    if (currentPage == NOTIFICATIONS)
    {
        ShowNotifications();
        return;
    }

    if (currentPage == FINANCE)
    {
        string command = Lower(a);
        int deleteId = DeleteIndexFromText(a);

        if (command == "clear")
        {
            incomeValue = 0;
            expenseValue = 0;
            financeLogs.clear();

            SaveFinance();
            SaveFinanceLog();
            ShowFinance();
            return;
        }

        if (deleteId > 0)
        {
            if (deleteId <= (int)financeLogs.size())
            {
                FinanceItem old = financeLogs[deleteId - 1];

                if (old.type == "income")
                    incomeValue -= old.amount;

                if (old.type == "expense")
                    expenseValue -= old.amount;

                if (incomeValue < 0)
                    incomeValue = 0;

                if (expenseValue < 0)
                    expenseValue = 0;

                financeLogs.erase(financeLogs.begin() + deleteId - 1);

                SaveFinance();
                SaveFinanceLog();
                ShowFinance();
                return;
            }

            SetPageText("FINANCE", FinanceText() + "\n\nNexus says:\nInvalid finance ID.");
            return;
        }

        double amount = atof(b.c_str());

        if (a.empty() || amount <= 0)
        {
            SetPageText("FINANCE", FinanceText() + "\n\nNexus says:\nDescription and positive amount are required.");
            return;
        }

        string type = Lower(c);

        FinanceItem item;
        item.date = NowText();
        item.amount = amount;
        item.description = a;

        if (type.find("gider") != string::npos || type.find("expense") != string::npos || type.find("out") != string::npos)
        {
            item.type = "expense";
            expenseValue += amount;
        }
        else
        {
            item.type = "income";
            incomeValue += amount;
        }

        financeLogs.push_back(item);

        SaveFinance();
        SaveFinanceLog();
        ShowFinance();
        return;
    }

    if (currentPage == SPORTS)
    {
        SetPageText("SPORTS", "Fetching sports data...\nPlease wait.");

        string text = FetchSportsText();

        sportsLastUpdate = NowText();
        SaveLastUpdates();

        SetPageText("SPORTS", SportsPageTextFromData(text));

        UpdateDashboardCards();
        ShowRunButton(true);
        SetRunText("Refresh");

        return;
    }

    if (currentPage == WEATHER)
    {
        if (!a.empty())
            cityName = a;

        SaveSettings();

        SetPageText("WEATHER", "Fetching weather data with WinHTTP...\nPlease wait.");

        string text = FetchWeatherText(cityName);
        text = CleanText(text);

        weatherLastUpdate = NowText();
        SaveLastUpdates();

        SetPageText("WEATHER", text + LastUpdateLine("Weather", weatherLastUpdate));

        UpdateDashboardCards();
        SetRunText("Refresh Weather");

        return;
    }

    if (currentPage == VEHICLE)
    {
        currentKm = atoll(a.c_str());
        lastServiceKm = atoll(b.c_str());
        serviceIntervalKm = atoll(c.c_str());
        fuelTotalCost = atof(d.c_str());

        if (serviceIntervalKm <= 0)
            serviceIntervalKm = 10000;

        if (fuelTotalCost < 0)
            fuelTotalCost = 0;

        SaveVehicle();
        ShowVehicle();
        return;
    }

    if (currentPage == PORTFOLIO)
    {
        cashTL = atof(a.c_str());
        usdAmount = atof(b.c_str());
        euroAmount = atof(c.c_str());
        goldGram = atof(d.c_str());

        if (cashTL < 0)
            cashTL = 0;

        if (usdAmount < 0)
            usdAmount = 0;

        if (euroAmount < 0)
            euroAmount = 0;

        if (goldGram < 0)
            goldGram = 0;

        SavePortfolio();
        ShowPortfolio();
        UpdateDashboardCards();
        return;
    }

    if (currentPage == WORKOUT)
    {
        weightKg = WeightInputToKg(atof(a.c_str()));
        heightCm = atof(b.c_str());
        targetWeightKg = WeightInputToKg(atof(c.c_str()));
        weeklyWorkoutGoal = atoi(d.c_str());

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

        SaveWorkout();
        ShowWorkout();
        return;
    }

    if (currentPage == AI)
    {
        EnterPage(AI, "AI ASSISTANT", AIAnswer(a));
        SetInputs("Command", a, "", "", "", "", "", "");
        SetRunText("Ask Nexus");
        return;
    }

    if (currentPage == SETTINGS)
    {
        timeFormatMode = NormalizeTimeFormat(a);
        weightUnitMode = NormalizeWeightUnit(b);

        SaveSettings();
        RefreshTopClock();
        ShowSettings();
        return;
    }

    ShowDashboard();
}