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
    const string headderStr = "#ifndef __htmls1212123___\n#define __htmls1212123___\n#include <Arduino.h>\nconst char htmlData[] PROGMEM = R\"=====(\n";
    const string footerStr = ")=====\";\n#endif\n";

    std::vector<std::string> allArgs(argv + 1, argv + argc);
    string finalString;
    char fArgStr[256];
    char inputFileName[128] = {0};
    char cssFileName[128] = {0};
    char outputFileName[128] = {0};
    fstream pInputFile;
    fstream pCSSInput;
    fstream pOutputFile;

    bool neFlag = false, ndeFlag = false, inFlag = false, outFlag = false;

    auto inIterator = std::find(std::begin(allArgs), std::end(allArgs), "-i");
    auto outIterator = std::find(std::begin(allArgs), std::end(allArgs), "-o");
    auto stdIterator = std::find(std::begin(allArgs), std::end(allArgs), "-ne");
    if (stdIterator != std::end(allArgs)) neFlag = 1;
    stdIterator = std::find(std::begin(allArgs), std::end(allArgs), "-nde");
    if (stdIterator != std::end(allArgs)) ndeFlag = 1;

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

    finalString = headderStr;

    pInputFile.open(inputFileName, ios::in);
    if (pInputFile.is_open()) {
        string readString;
        size_t foundCSS;

        while (getline(pInputFile, readString)) {
            foundCSS = readString.find("<link rel=\"stylesheet\" href=\"");
            if (foundCSS == string::npos) {
                finalString += readString;
                finalString += "\n";
            } else {
                if (extractCSSFileName(readString, cssFileName) != 0) {
                    cout << "Error getting CSS input file!" << endl;
                    pInputFile.close();
                    return 1;
                }
                pCSSInput.open(cssFileName, ios::in);
                if (pCSSInput.is_open()) {
                    finalString += "<style>\n";
                    while (getline(pCSSInput, readString)) {
                        finalString += readString;
                    }
                    finalString += "</style>\n";
                    pCSSInput.close();
                } else {
                    cout << "Error opening CSS input file: " << cssFileName << endl;
                    pInputFile.close();
                    return 1;
                }
            }
        }
        pInputFile.close();
    } else {
        cout << "Error opening input file: " << inputFileName << endl;
        return 1;
    }

    finalString += footerStr;

    if (!neFlag) {
        if (!ndeFlag) {
            cout << "final string:" << endl;
        }
        cout << finalString << endl;
    }

    return 0;
}

int extractCSSFileName(string inStr, char *retStr) {
    size_t found1;
    size_t found2;
    found1 = inStr.find("<link rel=\"stylesheet\"  href=\"");
    found1 += strlen("<link rel=\"stylesheet\"  href=\"");
    found2 = inStr.find(".css\"", found1);
    if (found2 == string::npos) {
        cout << "No valid CSS file name found!" << endl;
        return 1;
    }
    inStr.copy(retStr, found2 - found1 + 2, found1 + 2);
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
