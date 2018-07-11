/*
    A plugin for macOS software 'bitbar'.
    Displays date/time on the bar
    And displays a calendar on expanding.
 */


#include <iostream>
#include <ctime>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
using namespace std;

string exec(const char*);
string fill2(int);
string pad2(int);
vector<string> createCal(int, int, int, string);
string setAttr(string, string);
string resetAttr();

string mono_format = "| font=\'DejaVu Sans Mono for Powerline\' trim=false color=white";

string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}

string fill2(int num) {
    if (num >= 10) return to_string(num);
    else return "0" + to_string(num);
}
string pad2(int num) {
    if (num >= 10) return to_string(num);
    else return " " + to_string(num);
}

vector<string> createCal(int firstDOW, int monthLen, int today, string format) {
    vector<string> results;
    results.push_back(setAttr("white", "") + " Mo Tu We Th Fr " + setAttr("red", "") + "Sa Su " + format);
    int dateCounter = 1;
    int placeCounter = 0;
    string nextSplit = " "; // for entering the "today bracket"
    string nowRow = setAttr("white", "");
    for (int i = 0; i < firstDOW - 1; i++){
        nowRow += "   ";
        placeCounter += 1;
    }
    while (true) {
        if (today == dateCounter) {
            nowRow += setAttr("green", "");
            nowRow += "<";
            nowRow += pad2(dateCounter);
            nextSplit = ">";
        }
        else {
            nowRow += nextSplit;
            if (placeCounter == 5 || placeCounter == 6)
                nowRow += setAttr("red", "");
            else
                nowRow += setAttr("white", "");
            nowRow += pad2(dateCounter);
            nextSplit = " ";
        }
        dateCounter += 1;
        placeCounter += 1;

        if (placeCounter >= 7) {
            if (nextSplit == ">") {
                nowRow += nextSplit;
                nextSplit = " ";
            }
            nowRow += format;
            results.push_back(nowRow);
            nowRow = setAttr("white", "");
            placeCounter = 0;
        }
        if (dateCounter > monthLen) {
            if (nextSplit == ">") {
                nowRow += nextSplit;
            }
            nowRow += format;
            results.push_back(nowRow);
            break;
        }
    }
    return results;
}

// change the output string attribute
string setAttr(string color, string format) {
    string result = "";
    bool hasColorAttr = true;
    bool hasFormatAttr = false;
    result += "\033[";
    if (color == "black")
        result += "30;";
    else if (color == "red")
        result += "31;";
    else if (color == "green")
        result += "32;";
    else if (color == "yellow")
        result += "33;";
    else if (color == "blue")
        result += "34;";
    else if (color == "magenta")
        result += "35;";
    else if (color == "cyan")
        result += "36;";
    else if (color == "white")
        result += "37;";
    else {
        result += ";";
        hasColorAttr = false;
    }

    if (format.find("r") != string::npos){ // reset
        result += "0;";
        hasFormatAttr = true;
    }
    if (format.find("b") != string::npos){ // bold
        result += "1;";
        hasFormatAttr = true;
    }
    if (format.find("u") != string::npos){ // underline
        result += "4;";
        hasFormatAttr = true;
    }
    if (format.find("i") != string::npos){ // inverse
        result += "7;";
        hasFormatAttr = true;
    }
    
    if (!hasColorAttr && !hasFormatAttr) {
        return "";
    } else {
        result = result.substr(0, result.size() - 1);
        result += "m";
        return result;
    }
}

// reset all output string attributes
string resetAttr(){
    return "\033[0m";
}

int main(int argc, const char* argv[]) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    tm.tm_year += 1900;
    tm.tm_mon += 1;
    int dayOfWeek = stoi(exec("date +%u")); // get today day of week
    int firstDayOfWeek = stoi(exec("date -v1d +%u")); // get the day of week of first day of this month
    int totalDaysOfMonth = stoi(exec("date -v1d -v+1m -v-1d +%d")); // get how much day this month has

    cout << setAttr("white", "");
    cout << fill2(tm.tm_mon) << fill2(tm.tm_mday);
    cout << setAttr("yellow", "u") << "(" << dayOfWeek << ")" << resetAttr();
    cout << setAttr("white", ""); 
    cout << fill2(tm.tm_hour) << fill2(tm.tm_min) << mono_format << endl;
    cout << "---" << endl;

    // calendar
    cout << tm.tm_year << "/" << tm.tm_mon << mono_format << endl;
    vector<string> calendar = createCal(firstDayOfWeek, totalDaysOfMonth, tm.tm_mday, mono_format);
    for (int i = 0; i < calendar.size(); i++) {
        cout << calendar[i] << endl;
    }
    return 0;
}