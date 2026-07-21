#include "app.h"

static void SetCtrlVisible(int handle, bool visible)
{
    HWND h = ICG_GetHWND(handle);

    if (!h)
        return;

    ShowWindow(h, visible ? SW_SHOW : SW_HIDE);
}

static void SetCtrlTextValue(int handle, string text)
{
    HWND h = ICG_GetHWND(handle);

    if (!h)
        return;

    SetWindowTextA(h, text.c_str());
    InvalidateRect(h, NULL, TRUE);
    UpdateWindow(h);
}

static void SetCtrlBounds(int handle, int x, int y, int w, int h)
{
    HWND hwnd = ICG_GetHWND(handle);

    if (!hwnd)
        return;

    MoveWindow(hwnd, x, y, w, h, TRUE);
    InvalidateRect(hwnd, NULL, TRUE);
    UpdateWindow(hwnd);
}

static bool HasText(string s)
{
    return !Trim(s).empty();
}

static int VisibleInputCount(string a, string b, string c, string d)
{
    int n = 0;

    if (HasText(a)) n++;
    if (HasText(b)) n++;
    if (HasText(c)) n++;
    if (HasText(d)) n++;

    return n;
}

static void HideAllInputs()
{
    SetCtrlVisible(label1, false);
    SetCtrlVisible(label2, false);
    SetCtrlVisible(label3, false);
    SetCtrlVisible(label4, false);

    SetCtrlVisible(edit1, false);
    SetCtrlVisible(edit2, false);
    SetCtrlVisible(edit3, false);
    SetCtrlVisible(edit4, false);
}

static void LayoutInputRow(int visibleCount)
{
    int panelLeft = 335;
    int topLabel = 604;
    int topEdit = 638;

    int labelHeight = 28;
    int editHeight = 40;

    int gap = 18;

    int buttonWidth = 215;
    int buttonHeight = 46;
    int buttonX = 1135;
    int buttonY = 634;

    int usableWidth = buttonX - panelLeft - 28;

    int fieldWidth = 190;

    if (visibleCount == 1) fieldWidth = 300;
    if (visibleCount == 2) fieldWidth = 255;
    if (visibleCount == 3) fieldWidth = 215;
    if (visibleCount >= 4) fieldWidth = 185;

    int totalWidth = visibleCount * fieldWidth + (visibleCount - 1) * gap;

    if (totalWidth > usableWidth)
        fieldWidth = (usableWidth - (visibleCount - 1) * gap) / visibleCount;

    int x = panelLeft;

    vector<int> labels;
    vector<int> edits;

    HWND h1 = ICG_GetHWND(label1);
    HWND h2 = ICG_GetHWND(label2);
    HWND h3 = ICG_GetHWND(label3);
    HWND h4 = ICG_GetHWND(label4);

    if (h1 && IsWindowVisible(h1))
    {
        labels.push_back(label1);
        edits.push_back(edit1);
    }

    if (h2 && IsWindowVisible(h2))
    {
        labels.push_back(label2);
        edits.push_back(edit2);
    }

    if (h3 && IsWindowVisible(h3))
    {
        labels.push_back(label3);
        edits.push_back(edit3);
    }

    if (h4 && IsWindowVisible(h4))
    {
        labels.push_back(label4);
        edits.push_back(edit4);
    }

    for (int i = 0; i < (int)labels.size(); i++)
    {
        SetCtrlBounds(labels[i], x, topLabel, fieldWidth, labelHeight);
        SetCtrlBounds(edits[i], x, topEdit, fieldWidth, editHeight);
        x += fieldWidth + gap;
    }

    SetCtrlBounds(runButtonHandle, buttonX, buttonY, buttonWidth, buttonHeight);
}

void ShowInputPair(int lab, int edit, bool show)
{
    SetCtrlVisible(lab, show);
    SetCtrlVisible(edit, show);
}

void ShowRunButton(bool show)
{
    SetCtrlVisible(runButtonHandle, show);
}

void SetRunText(string text)
{
    if (!runButtonHandle)
        return;

    CreateImage(runImg, 215, 46, ICB_UINT);

    FillRoundRect(runImg, 0, 0, 215, 46, 12, C_ACTIVE);
    Line(runImg, 12, 1, 202, 1, C_CYAN);
    Line(runImg, 12, 44, 202, 44, C_BORDER);
    Line(runImg, 1, 12, 1, 33, C_BORDER);
    Line(runImg, 213, 12, 213, 33, C_CYAN);

    int x = 107 - ((int)text.size() * 5);

    if (x < 14)
        x = 14;

    DrawTextWIN(runImg, x, 12, text.c_str(), C_CYAN, 20, 0, "Consolas");

    SetButtonBitmap(runButtonHandle, runImg);

    HWND h = ICG_GetHWND(runButtonHandle);

    if (h)
    {
        InvalidateRect(h, NULL, TRUE);
        UpdateWindow(h);
    }
}

void SetInputs(string l1, string v1, string l2, string v2, string l3, string v3, string l4, string v4)
{
    HideAllInputs();

    if (HasText(l1))
    {
        SetCtrlTextValue(label1, l1);
        SetCtrlTextValue(edit1, v1);
        SetCtrlVisible(label1, true);
        SetCtrlVisible(edit1, true);
    }

    if (HasText(l2))
    {
        SetCtrlTextValue(label2, l2);
        SetCtrlTextValue(edit2, v2);
        SetCtrlVisible(label2, true);
        SetCtrlVisible(edit2, true);
    }

    if (HasText(l3))
    {
        SetCtrlTextValue(label3, l3);
        SetCtrlTextValue(edit3, v3);
        SetCtrlVisible(label3, true);
        SetCtrlVisible(edit3, true);
    }

    if (HasText(l4))
    {
        SetCtrlTextValue(label4, l4);
        SetCtrlTextValue(edit4, v4);
        SetCtrlVisible(label4, true);
        SetCtrlVisible(edit4, true);
    }

    int count = VisibleInputCount(l1, l2, l3, l4);

    if (count == 0)
    {
        ShowRunButton(false);
        return;
    }

    ShowRunButton(true);
    LayoutInputRow(count);
}

void ClearAllEdits()
{
    SetCtrlTextValue(edit1, "");
    SetCtrlTextValue(edit2, "");
    SetCtrlTextValue(edit3, "");
    SetCtrlTextValue(edit4, "");
}

void RefreshFormLayout()
{
    int count = 0;

    HWND h1 = ICG_GetHWND(label1);
    HWND h2 = ICG_GetHWND(label2);
    HWND h3 = ICG_GetHWND(label3);
    HWND h4 = ICG_GetHWND(label4);

    if (h1 && IsWindowVisible(h1)) count++;
    if (h2 && IsWindowVisible(h2)) count++;
    if (h3 && IsWindowVisible(h3)) count++;
    if (h4 && IsWindowVisible(h4)) count++;

    if (count == 0)
    {
        ShowRunButton(false);
        return;
    }

    ShowRunButton(true);
    LayoutInputRow(count);
}