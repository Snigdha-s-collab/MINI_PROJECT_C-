#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <functional>
#include <limits>
#include <cctype>
#include <map>
#include <queue>
#include <set>
#include <fstream>
#include <numeric>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// ==================== ANSI COLOR CODES ====================
namespace Color {
    const string RESET   = "\033[0m";
    const string BOLD    = "\033[1m";
    const string DIM     = "\033[2m";
    const string ITALIC  = "\033[3m";
    const string ULINE   = "\033[4m";
    const string RED     = "\033[31m";
    const string GREEN   = "\033[32m";
    const string YELLOW  = "\033[33m";
    const string BLUE    = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN    = "\033[36m";
    const string WHITE   = "\033[37m";
    const string BRED    = "\033[91m";
    const string BGREEN  = "\033[92m";
    const string BYELLOW = "\033[93m";
    const string BBLUE   = "\033[94m";
    const string BMAGENTA= "\033[95m";
    const string BCYAN   = "\033[96m";
    const string BG_BLUE = "\033[44m";
    const string BG_GREEN= "\033[42m";
    const string BG_RED  = "\033[41m";
    const string BG_CYAN = "\033[46m";
    const string BG_MAG  = "\033[45m";
}

// ==================== CONSOLE UTILITIES ====================
namespace ConsoleUI {
    inline void enableColors() {
#ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode |= 0x0004; // ENABLE_VIRTUAL_TERMINAL_PROCESSING
        SetConsoleMode(hOut, dwMode);
#endif
    }

    inline void clearScreen() {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    inline string repeat(const string& s, int n) {
        string r;
        for (int i = 0; i < n; i++) r += s;
        return r;
    }

    inline void printHeader(const string& title) {
        int w = 60;
        string border = "+" + repeat("-", w - 2) + "+";
        int pad = (w - 4 - (int)title.size()) / 2;
        if (pad < 0) pad = 0;
        cout << "\n" << Color::BCYAN << Color::BOLD;
        cout << border << "\n";
        cout << "| " << repeat(" ", pad) << title << repeat(" ", w - 4 - pad - (int)title.size()) << " |\n";
        cout << border << Color::RESET << "\n\n";
    }

    inline void printSubHeader(const string& title) {
        cout << "\n" << Color::BYELLOW << Color::BOLD << "  --- " << title << " ---" << Color::RESET << "\n\n";
    }

    inline void printSuccess(const string& msg) {
        cout << Color::BGREEN << "  [OK] " << msg << Color::RESET << "\n";
    }

    inline void printError(const string& msg) {
        cout << Color::BRED << "  [!!] " << msg << Color::RESET << "\n";
    }

    inline void printWarning(const string& msg) {
        cout << Color::BYELLOW << "  [!] " << msg << Color::RESET << "\n";
    }

    inline void printInfo(const string& msg) {
        cout << Color::BCYAN << "  [i] " << msg << Color::RESET << "\n";
    }

    inline void printMenuItem(int num, const string& label) {
        cout << Color::CYAN << "    [" << Color::BYELLOW << num << Color::CYAN << "] " << Color::WHITE << label << Color::RESET << "\n";
    }

    inline void printDivider() {
        cout << Color::DIM << "  " << repeat("-", 50) << Color::RESET << "\n";
    }

    inline void printDoubleDivider() {
        cout << Color::DIM << "  " << repeat("=", 56) << Color::RESET << "\n";
    }

    inline void printStars(float rating) {
        int full = (int)rating;
        bool half = (rating - full) >= 0.5;
        string s;
        for (int i = 0; i < full; i++) s += "*";
        if (half) s += "+";
        for (int i = full + (half ? 1 : 0); i < 5; i++) s += ".";
        cout << Color::BYELLOW << s << Color::RESET;
    }

    inline void printProgressBar(int value, int maxVal, int width = 20) {
        int filled = (maxVal > 0) ? (value * width / maxVal) : 0;
        if (filled > width) filled = width;
        cout << Color::CYAN << "[";
        for (int i = 0; i < width; i++) {
            if (i < filled) cout << Color::BGREEN << "#";
            else cout << Color::DIM << "-";
        }
        cout << Color::CYAN << "] " << Color::WHITE << value << "%" << Color::RESET;
    }

    inline void pressEnter() {
        cout << Color::DIM << "\n  Press Enter to continue..." << Color::RESET;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    inline int getChoice(int minVal, int maxVal) {
        int c;
        while (true) {
            cout << Color::BMAGENTA << "  >> " << Color::RESET;
            if (cin >> c && c >= minVal && c <= maxVal) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return c;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid choice. Try again.");
        }
    }

    inline string getInput(const string& prompt) {
        cout << Color::CYAN << "  " << prompt << ": " << Color::WHITE;
        string s;
        getline(cin, s);
        cout << Color::RESET;
        return s;
    }

    inline float getFloat(const string& prompt, float minV = 0, float maxV = 1e9) {
        float v;
        while (true) {
            cout << Color::CYAN << "  " << prompt << ": " << Color::WHITE;
            if (cin >> v && v >= minV && v <= maxV) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return v;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input. Range: [" + to_string((int)minV) + "-" + to_string((int)maxV) + "]");
        }
    }

    inline int getInt(const string& prompt, int minV = 0, int maxV = 999999) {
        int v;
        while (true) {
            cout << Color::CYAN << "  " << prompt << ": " << Color::WHITE;
            if (cin >> v && v >= minV && v <= maxV) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return v;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printError("Invalid input. Range: [" + to_string(minV) + "-" + to_string(maxV) + "]");
        }
    }

    // Confirm yes/no prompt
    inline bool confirm(const string& prompt) {
        cout << Color::BYELLOW << "  " << prompt << " (y/n): " << Color::WHITE;
        string yn;
        getline(cin, yn);
        cout << Color::RESET;
        return (!yn.empty() && (yn[0] == 'y' || yn[0] == 'Y'));
    }

    // Print a formatted table row
    inline void printTableRow(const vector<string>& cols, const vector<int>& widths) {
        cout << "  " << Color::WHITE;
        for (size_t i = 0; i < cols.size(); i++) {
            int w = (i < widths.size()) ? widths[i] : 15;
            string val = cols[i];
            if ((int)val.size() > w) val = val.substr(0, w - 2) + "..";
            cout << left << setw(w) << val;
        }
        cout << Color::RESET << "\n";
    }

    inline void printTableHeader(const vector<string>& cols, const vector<int>& widths) {
        cout << "  " << Color::BOLD << Color::BCYAN;
        for (size_t i = 0; i < cols.size(); i++) {
            int w = (i < widths.size()) ? widths[i] : 15;
            cout << left << setw(w) << cols[i];
        }
        cout << Color::RESET << "\n  " << Color::DIM;
        for (size_t i = 0; i < widths.size(); i++) cout << repeat("-", widths[i]);
        cout << Color::RESET << "\n";
    }
}

// ==================== DATE UTILITIES ====================
namespace DateUtil {
    inline string getCurrentDate() {
        time_t now = time(0);
        tm* t = localtime(&now);
        char buf[11];
        snprintf(buf, 11, "%02d-%02d-%04d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);
        return string(buf);
    }

    inline string getCurrentTimestamp() {
        time_t now = time(0);
        tm* t = localtime(&now);
        char buf[20];
        snprintf(buf, 20, "%02d-%02d-%04d %02d:%02d", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min);
        return string(buf);
    }

    // Parse DD-MM-YYYY to time_t
    inline time_t parseDate(const string& d) {
        if (d.size() < 10) return 0;
        tm t = {};
        t.tm_mday = stoi(d.substr(0, 2));
        t.tm_mon = stoi(d.substr(3, 2)) - 1;
        t.tm_year = stoi(d.substr(6, 4)) - 1900;
        return mktime(&t);
    }

    inline bool isDeadlinePassed(const string& deadline) {
        return difftime(time(0), parseDate(deadline)) > 0;
    }

    inline int daysRemaining(const string& deadline) {
        double diff = difftime(parseDate(deadline), time(0));
        return (diff > 0) ? (int)(diff / 86400) + 1 : 0;
    }

    inline bool isValidDate(const string& d) {
        if (d.size() != 10 || d[2] != '-' || d[5] != '-') return false;
        try {
            int day = stoi(d.substr(0, 2));
            int mon = stoi(d.substr(3, 2));
            int yr = stoi(d.substr(6, 4));
            return day >= 1 && day <= 31 && mon >= 1 && mon <= 12 && yr >= 2024;
        } catch (...) { return false; }
    }
}

// ==================== STRING UTILITIES ====================
namespace StrUtil {
    inline string toLower(string s) {
        transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

    inline string trim(const string& s) {
        size_t a = s.find_first_not_of(" \t\r\n");
        size_t b = s.find_last_not_of(" \t\r\n");
        return (a == string::npos) ? "" : s.substr(a, b - a + 1);
    }

    inline vector<string> split(const string& s, char delim) {
        vector<string> parts;
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim)) {
            string trimmed = trim(item);
            if (!trimmed.empty()) parts.push_back(trimmed);
        }
        return parts;
    }

    inline string join(const vector<string>& v, const string& delim) {
        string r;
        for (size_t i = 0; i < v.size(); i++) {
            if (i > 0) r += delim;
            r += v[i];
        }
        return r;
    }

    // Simple djb2 hash for password hashing (demonstrates hashing concept)
    inline string hashPassword(const string& pwd) {
        unsigned long hash = 5381;
        for (char c : pwd) hash = ((hash << 5) + hash) + c;
        stringstream ss;
        ss << hex << hash;
        return ss.str();
    }

    // Mask password input display
    inline string maskString(const string& s) {
        return string(s.size(), '*');
    }

    // Check if a string contains another (case-insensitive)
    inline bool containsIgnoreCase(const string& haystack, const string& needle) {
        return toLower(haystack).find(toLower(needle)) != string::npos;
    }
}

// ==================== ID GENERATORS ====================
namespace IDGen {
    inline int nextID(int current) {
        return current + 1;
    }
}
