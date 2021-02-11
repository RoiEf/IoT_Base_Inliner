#include "inliner.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    const string headderStr = "#pragma once\n#include <Arduino.h>\nconst char htmlData[] PROGMEM = R\"=====(\n";
    const string footerStr = ")=====\";\n";

    std::vector<std::string> allArgs(argv + 1, argv + argc);
    string finalString;
    char fArgStr[256];
    char inputFileName[128] = {0};
    char genFileName[128] = {0};
    char outputFileName[128] = {0};
    fstream pInputFile;
    fstream pCSSInput;
    ofstream pOutputFile;

    bool neFlag = false, ndeFlag = false, inFlag = false, outFlag = false, ahgFlag = false;

    auto inIterator = std::find(std::begin(allArgs), std::end(allArgs), "-i");
    auto outIterator = std::find(std::begin(allArgs), std::end(allArgs), "-o");
    auto stdIterator = std::find(std::begin(allArgs), std::end(allArgs), "-ne");
    if (stdIterator != std::end(allArgs)) neFlag = 1;
    stdIterator = std::find(std::begin(allArgs), std::end(allArgs), "-nde");
    if (stdIterator != std::end(allArgs)) ndeFlag = 1;
    stdIterator = std::find(std::begin(allArgs), std::end(allArgs), "-ahg");
    if (stdIterator != std::end(allArgs)) ahgFlag = true;

    if (!ndeFlag) {
        cout
            << "You have entered " << argc
            << " arguments:"
            << "\n";

        for (int i = 0; i < argc; ++i)
            cout << i << ": " << argv[i] << "\n";

        parseFirstArg(argv[0], fArgStr);
        cout << "Hello " << fArgStr << endl;
    }

    if (inIterator != std::end(allArgs)) {
        int i = std::distance(allArgs.begin(), inIterator) + 1;
        strcpy(inputFileName, allArgs[i].c_str());
        inFlag = true;
    } else
        strcpy(inputFileName, "index.html");

    if (outIterator != std::end(allArgs)) {
        int i = std::distance(allArgs.begin(), outIterator) + 1;
        strcpy(outputFileName, allArgs[i].c_str());
        outFlag = true;
    }

    if (!ndeFlag) {
        cout << "inputFileName: " << inputFileName << endl;
        cout << "outputFileName: " << outputFileName << endl;
    }

    if (ahgFlag) {
        finalString = headderStr;
    }

    pInputFile.open(inputFileName, ios::in);
    if (!pInputFile.is_open()) {
        cout << "Error opening input file: " << inputFileName << endl;
        return 1;
    } else {
        string readString;
        size_t foundCSS;

        while (getline(pInputFile, readString)) {
            foundCSS = readString.find("<link rel=\"stylesheet\" href=\"");
            if (foundCSS != string::npos) {
                if (extractFileName(readString, genFileName) != 0) {
                    cout << "Error getting CSS input file!" << endl;
                    pInputFile.close();
                    return 1;
                }
                pCSSInput.open(genFileName, ios::in);
                if (!pCSSInput.is_open()) {
                    cout << "Error opening CSS input file: " << genFileName << endl;
                    pInputFile.close();
                    return 1;
                } else {
                    finalString += "<style>";
                    while (getline(pCSSInput, readString)) {
                        finalString += readString;
                    }
                    finalString += "</style>\n";
                    pCSSInput.close();
                }
            } else {
                foundCSS = readString.find("<script type=\"module\" src=\"");
                if (foundCSS != string::npos) {
                    if (extractFileName(readString, genFileName) != 0) {
                        cout << "Error getting Script input file!" << endl;
                        pInputFile.close();
                        return 1;
                    }
                    pCSSInput.open(genFileName, ios::in);
                    if (!pCSSInput.is_open()) {
                        cout << "Error opening Script input file: " << genFileName << endl;
                        pInputFile.close();
                        return 1;
                    } else {
                        finalString += "<script type=\"module\">";
                        while (getline(pCSSInput, readString)) {
                            finalString += readString;
                        }
                        finalString += "</script>\n";
                        pCSSInput.close();
                    }
                } else {
                    finalString += readString;
                    finalString += "\n";
                }
            }
        }
        pInputFile.close();
    }

    if (ahgFlag) {
        finalString += footerStr;
    }

    if (!neFlag) {
        if (!ndeFlag) {
            cout << "final string:" << endl;
        }
        cout << finalString << endl;
    }
    if (outFlag) {
        pOutputFile.open(outputFileName);
        if (pOutputFile.is_open()) {
            pOutputFile << finalString;
            pOutputFile.close();
        } else {
            cout << "Error opening output file: " << outputFileName << endl;
            return 1;
        }
    }

    return 0;
}

int extractFileName(string inStr, char *retStr) {
    size_t found1;
    size_t found2;
    found1 = inStr.find("<link rel=\"stylesheet\" href=\"");
    if (found1 != string::npos) {
        found1 += strlen("<link rel=\"stylesheet\" href=\"");
        found2 = inStr.find(".css\"", found1);
        if (found2 == string::npos) {
            cout << "No valid CSS file name found!" << endl;
            return 1;
        }
        inStr.copy(retStr, found2 - found1 + 4, found1);
    } else {
        found1 = inStr.find("<script type=\"module\" src=\"");
        if (found1 == string::npos) {
            cout << "No valid script file name found!" << endl;
            return 1;
        } else {
            found1 += strlen("<script type=\"module\" src=\"");
            found2 = inStr.find(".js\"", found1);
            if (found2 == string::npos) {
                cout << "No valid script file name found!" << endl;
                return 1;
            }
            inStr.copy(retStr, found2 - found1 + 3, found1);
        }
    }

    return 0;
}

void parseFirstArg(char *argStr, char *retStr) {
    int len, i, n;
    // char *startWord;

    // startWord = argStr;
    len = strlen(argStr);
    for (i = 3, n = 0; i < len - 4; i++, n++) {
        if (argStr[i] == '\\') {
            // i++;
            n = -1;
            // n = i;
            // startWord = &argStr[i];
        } else {
            retStr[n] = argStr[i];
        }
    }
    // for (i = 0, n; n < len - 4; n++, i++) {
    //     retStr[i] = argStr[n];
    // }
    retStr[n] = '\0';

    // cout << "argStr: " << argStr << endl;
    // cout << "retStr: " << retStr << endl;
}
