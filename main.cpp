#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include <limits> // For numeric_limits
using namespace std;
map<string, int> primaryIndex;
map<string, int> primaryIndexOnAppoint;
vector<pair<int,int>> doctorAvailList;
vector<pair<int,int>> appointmentAvailList;
struct Record{  //to implement read and write and delete record
    string s1;
    string s2;
    string s3;

Record readRecord(fstream &file, int offset) {
    file.seekg(offset, ios::beg);//move read pointer to the begin of the record you want to read
    int recordSize;// Read record size and skip it
    file >> recordSize;
    // Read the fields
    string s1, s2, s3;
    getline(file, s1, '|');
    getline(file, s2, '|');
    getline(file, s3, '|');

    return {s1, s2, s3}; // Return as a Record struct
}
void deleteRecord(fstream &file, int offset, vector<pair<int,int>> &availList) {
    // Read the record to find its length
    file.seekg(offset, ios::beg);
    int recordSize;
    file >> recordSize;
    // Move to the end of the record and append * overwrite the last char with *
    file.seekp(offset + recordSize , ios::beg);
    file.put('*'); 
    // Add the record size and offset to the avail list
    availList.emplace_back(recordSize, offset);
    // Sort the avail list by record size (ascending) to implement bestfit strategy
    sort(availList.begin(), availList.end());

}
void saveAvailListToFile(const vector<pair<int, int>> &availList, const string &filename) {
    ofstream outfile(filename, ios::out);
    if (!outfile) {
        cerr << "Error opening file to save avail list.\n";
        return;
    }
    for (const auto &entry : availList) {
        outfile << entry.first << " " << entry.second << "\n";
    }
    outfile.close();
    cout << "Avail list saved to " << filename << "\n";
}
void loadAvailListFromFile(vector<pair<int, int>> &availList, const string &filename) {
    ifstream infile(filename, ios::in);
    if (!infile) {
        cerr << "Error opening file to load avail list.\n";
        return;
    }
    availList.clear();// Clear the existing list to avoid appending to old data
    int recordSize, offset;
    while (infile >> recordSize >> offset) {
        availList.emplace_back(recordSize, offset);
    }
    infile.close();
    sort(availList.begin(), availList.end());
    cout << "Avail list loaded from " << filename << "\n";
}

};
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
    void deleteDoctor(const string &doctorID) {
        if (primaryIndex.find(doctorID) == primaryIndex.end()) {
            cout << "Doctor ID you want to delete does not exist. Deletion failed.\n";
            return;
        }

        fstream file("Doctor.txt", ios::in | ios::out);
        if (!file) {
            cerr << "Error opening file for deleting doctor.\n";
            return;
        }

        useprimaryIndex p ;
        int offset = p.binarySearchPrimaryIndex(primaryIndex,doctorID);
        Record r1;
        r1.deleteRecord(file, offset, doctorAvailList);
        p.removePrimaryIndex(primaryIndex,doctorID);// Remove from primary index
        cout << "Doctor record deleted successfully.\n";

        p.savePrimaryIndexToFile(primaryIndex, "PrimaryIndexOnDocID.txt");
        r1.saveAvailListToFile(doctorAvailList, "DoctorAvailList.txt");
    }
    void printDoctorInfo(const string& doctorID) {
        if (primaryIndex.find(doctorID) == primaryIndex.end()) {
            cout << "Doctor ID not found.\n";
            return;
        }
        useprimaryIndex p ;
        int offset = p.binarySearchPrimaryIndex(primaryIndex,doctorID);
        fstream file("Doctor.txt", ios::in);
        if (!file) {
            cerr << "Error opening file to read doctor information.\n";
            return;
        }
        Record record = record.readRecord(file, offset);
        file.close();
        cout << "Doctor Information:\n";
        cout << "Doctor ID: " << record.s1 << "\n";
        cout << "Doctor Name: " << record.s2 << "\n";
        cout << "Address: " << record.s3 << "\n";
    }
};
struct Appointment {
    char appointmentID[15];
    char appointmentDate[30];
    char doctorID[15];
    void deleteAppoint(const string &AppID) {
        if (primaryIndexOnAppoint.find(AppID) == primaryIndexOnAppoint.end()) {
            cout << "Appointment ID you want to delete does not exist. Deletion failed.\n";
            return; }
        fstream file("Appointment.txt", ios::in | ios::out);
        if (!file) {
            cerr << "Error opening file for deleting doctor.\n";
            return;
        }
        useprimaryIndex p ;
        int offset = p.binarySearchPrimaryIndex(primaryIndexOnAppoint,AppID);
        Record r2;
        r2.deleteRecord(file, offset, appointmentAvailList);
        p.removePrimaryIndex(primaryIndexOnAppoint,AppID);
        cout << "Appointment record deleted successfully.\n";

        p.savePrimaryIndexToFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
        r2.saveAvailListToFile(doctorAvailList, "DoctorAvailList.txt");
    }
    void printAppointmentInfo(const string& AppID) {
        if (primaryIndexOnAppoint.find(AppID) == primaryIndexOnAppoint.end()) {
            cout << "Appointment ID not found.\n";
            return;
        }
        useprimaryIndex p ;
        int offset = p.binarySearchPrimaryIndex(primaryIndexOnAppoint,AppID);
        fstream file("Appointment.txt", ios::in);
        if (!file) {
            cerr << "Error opening file to read Appointment information.\n";
            return;
        }
        Record record = record.readRecord(file, offset);
        file.close();
        cout << "Appointment Information:\n";
        cout << "Appointment ID: " << record.s1 << "\n";
        cout << "Appointment Date: " << record.s2 << "\n";
        cout << "Doctor ID: " << record.s3 << "\n";
    }
};

int main() {
    useprimaryIndex p;
    Record r1;
    p.loadPrimaryIndexFromFile(primaryIndex, "PrimaryIndexOnDocID.txt");
    p.loadPrimaryIndexFromFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
    r1.loadAvailListFromFile(doctorAvailList, "doctorAvailList.txt");
    r1.loadAvailListFromFile(appointmentAvailList,"appointmentAvailList.txt");
    while (true) {
        cout << "\nWelcome! to Healthcare management system\n";
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
        cout << "Enter your choice:\n";
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
        if (choice == 5) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string AppID;
            cout << "Enter Appointment ID to delete: ";
            cin >> AppID;
            Appointment app;
            app.deleteAppoint(AppID);
        }
        if (choice == 6) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string doctorID;
            cout << "Enter Doctor ID to delete: ";
            cin >> doctorID;
            Doctor doc;
            doc.deleteDoctor(doctorID);
        }
        if (choice == 7) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string doctorID;
            cout << "Enter Doctor ID to print information: ";
            cin >> doctorID;
            Doctor doc;
            doc.printDoctorInfo(doctorID);
        }

        if (choice == 8) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string AppID;
            cout << "Enter Appointment ID to print information: ";
            cin >> AppID;
            Appointment app;
            app.printAppointmentInfo(AppID);
        }
    }
}
