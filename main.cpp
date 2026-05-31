```cpp
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

const string DB_FILE = "donors.csv";

// Authenticate with ESP32 hardware key
string authenticateWithESP32() {
    HANDLE hSerial = CreateFile("\\\\.\\COM3",
        GENERIC_READ | GENERIC_WRITE,
        0, NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        cerr << "Error: Cannot open serial port." << endl;
        return "";
    }

    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);

    GetCommState(hSerial, &dcb);

    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.StopBits = ONESTOPBIT;
    dcb.Parity = NOPARITY;

    SetCommState(hSerial, &dcb);

    DWORD bytesWritten;
    string command = "SEND_KEY\n";

    WriteFile(hSerial, command.c_str(), command.size(), &bytesWritten, NULL);

    Sleep(500);

    char buffer[32] = {0};
    DWORD bytesRead;

    ReadFile(hSerial, buffer, 31, &bytesRead, NULL);

    CloseHandle(hSerial);

    string donorID(buffer);
    donorID.erase(donorID.find_last_not_of("\r\n ") + 1);

    return donorID;
}

// Initialize database
void initializeDatabase() {
    ifstream file(DB_FILE.c_str());

    if (!file.is_open()) {
        ofstream newFile(DB_FILE.c_str());

        newFile << "DonorID,FullName,BloodType,Age,Contact,City,LastDonationDate,TotalDonations,Eligible\n";

        newFile.close();

        cout << "Database initialized." << endl;
    }
    else {
        cout << "Database loaded." << endl;
        file.close();
    }
}

// Check unique ID
bool isUnique(string id) {
    ifstream file(DB_FILE.c_str());

    string line, fileID;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);

        getline(ss, fileID, ',');

        if (fileID == id) {
            file.close();
            return false;
        }
    }

    file.close();
    return true;
}

// Add new record
void appendRecord(string data) {
    ofstream file(DB_FILE.c_str(), ios::app);

    file << data << "\n";

    file.close();

    cout << "Record added successfully." << endl;
}

// Search donor by ID
void searchByID(string id) {
    ifstream file(DB_FILE.c_str());

    string line, fileID;

    bool found = false;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);

        getline(ss, fileID, ',');

        if (fileID == id) {
            cout << "Record Found: " << line << endl;
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Donor not found." << endl;
    }

    file.close();
}

// Update or delete record
void updateRecord(string id, string newData, bool del = false) {

    ifstream inputFile(DB_FILE.c_str());
    ofstream tempFile("temp.csv");

    string line, fileID;

    bool done = false;

    while (getline(inputFile, line)) {

        stringstream ss(line);

        getline(ss, fileID, ',');

        if (fileID == id) {

            if (!del) {
                tempFile << newData << "\n";
            }

            done = true;
        }
        else {
            tempFile << line << "\n";
        }
    }

    inputFile.close();
    tempFile.close();

    remove(DB_FILE.c_str());
    rename("temp.csv", DB_FILE.c_str());

    if (done)
        cout << "Operation complete." << endl;
    else
        cout << "ID not found." << endl;
}

int main() {

    initializeDatabase();

    string auth = authenticateWithESP32();

    if (auth == "") {
        return 0;
    }

    cout << "Hardware Authenticated: " << auth << endl;

    return 0;
}
```
