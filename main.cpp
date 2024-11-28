#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
map<string, int> primaryIndex;
map<string, int> primaryIndexOnAppoint;
struct useprimaryIndex {
    void addPrimaryIndex(map<string, int> &primaryIndexm, const string &key, int offset) {
        primaryIndexm[key] = offset;
    }

    void removePrimaryIndex(map<string, int> &primaryIndexm, const string &key) {
        primaryIndexm.erase(key);
    }

    int binarySearchPrimaryIndex(const map<string, int> &primaryIndexm, const string &key) {
        auto it = primaryIndexm.find(key); // Binary search happens internally in the map
        if (it != primaryIndexm.end()) {
            return it->second; // Return offset if key is found
        }
        return -1; // Return -1 if not found
    }

    void savePrimaryIndexToFile(const map<string, int> &primaryIndexm, const string &filename) {
        ofstream outfile(filename, ios::out);
        if (!outfile) {
            cerr << "Error opening file for saving primary index!" << endl;
            return;
        }
        for (const auto &entry: primaryIndexm) {
            outfile << entry.first << " " << entry.second << "\n";
        }
        outfile.close();
        cout << "Primary index saved to " << filename << endl;
    }

    void loadPrimaryIndexFromFile(map<string, int> &primaryIndex, const string &filename) {
        ifstream infile(filename, ios::in);
        if (!infile) {
            cerr << "Error opening file for loading primary index: " << filename << endl;
            return;
        }
        string key;
        int offset;
        while (infile >> key >> offset) {
            primaryIndex[key] = offset;
        }
        infile.close();
        cout << "Primary index loaded from " << filename << endl;
    }
};

struct Doctor {
    char doctorID[15];
    char doctorName[30];
    char address[30];
    void addDoctor(const string& doctorID, const string& name, const string& address,map<string, int>& primaryIndex) {
        if (primaryIndex.find(doctorID) != primaryIndex.end()) {
            cout << "Doctor ID already exists! Enter another ID" << endl;
            return;
        }
        fstream outfile;
        outfile.open("Doctor.txt", ios::in|ios::out|ios::app);
        int offset = 0;
        outfile.seekp(0, ios::end);  // Move to the end of the file to calculate offset
        offset = outfile.tellp();   //*****************************************************\\
        // Write record to file //**** THIS WHOLE PART WILL BE REPLACED WITH WRITE FUNCTION *****
        string named , id , add;
        named = name ; id = doctorID ; add = address;
        id.push_back('|');
        named.push_back('|');
        add.push_back('|');
        int sizeofname = named.size(); int sizeofid = id.size(); int sizeofadd = add.size(); int sizeOfRecord = sizeofname + sizeofid + sizeofadd;
        outfile << sizeOfRecord; outfile << id; outfile << named; outfile << add ;
        useprimaryIndex p;
        p.addPrimaryIndex(primaryIndex,doctorID,offset);
        // Save indexes
        p.savePrimaryIndexToFile(primaryIndex, "PrimaryIndexOnDocID.txt");
    }
};
struct Appointment {
    char appointmentID[15];
    char appointmentDate[30];
    char doctorID[15];
};


struct AvailListDOC {
    vector<int> offsets; // Track available space in the file.
    int getAvailableOffset(vector<int>& availList) {
        if (availList.empty()) return -1;
        int offset = availList.back();
        availList.pop_back();
        return offset;
    }

    void addToAvailList(vector<int>& availList, int offset) {
        availList.push_back(offset);
    }
};

int main() {
    useprimaryIndex p;
    p.loadPrimaryIndexFromFile(primaryIndex, "PrimaryIndexOnDocID.txt");
   // p.loadPrimaryIndexFromFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
    while (true) {
        cout << "\nWelcome! Enter your choice:\n";
        cout << "1. Add New Doctor\n";
        cout << "2. Add New Appointment\n";
        cout << "3. Update Doctor Name (Doctor ID)\n";
        cout << "4. Update Appointment Date (Appointment ID)\n";
        cout << "5. Delete Appointment (Appointment ID)\n";
        cout << "6. Delete Doctor (Doctor ID)\n";
        cout << "7. Print Doctor Info (Doctor ID)\n";
        cout << "8. Print Appointment Info (Appointment ID)\n";
        cout << "9. Write Query\n";
        cout << "0. Exit\n";
        int choice;
        cin >> choice;
        if (choice == 0) {
            cout << "Exiting the program.\n";
            break; }
        if (choice == 1) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
            Doctor doc;
            cout << "Enter data of the doctor:\n";
            cout << "Doctor Name:";
            cin.getline(doc.doctorName, size(doc.doctorName));
            cout << "Doctor ID:";
            cin >> doc.doctorID;
            cout << "Address:";
            cin.ignore();  // Clear the newline character left in the buffer
            cin.getline(doc.address, size(doc.address));
            doc.addDoctor(doc.doctorID, doc.doctorName, doc.address, primaryIndex);
        }
    }


}



