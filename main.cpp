#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include<map>
#include<vector>
#include<string>
#include <limits> // For numeric_limits
using namespace std;
map<string, int> primaryIndex;
map<string, int> primaryIndexOnAppoint;
vector<int> doctorAvailList;
vector<int> appointmentAvailList;
struct Record{  //to implement read and write and delete record
    string s1;
    string s2;
    string s3;
};
Record readRecord(fstream &file, int offset) {
    file.seekg(offset, ios::beg);//move read pointer to the begin of the record you want to read
    int recordSize;// Read record size and skip it
    file >> recordSize;
   // file.ignore(); // Skip the delimiter
    // Read the fields
    string s1, s2, s3;
    getline(file, s1, '|');
    getline(file, s2, '|');
    getline(file, s3, '|');

    return {s1, s2, s3}; // Return as a Record struct
}
void deleteRecord(fstream &file, int offset, vector<int> &availList) {
    // Read the record to find its length
    file.seekg(offset, ios::beg);
    int recordSize;
    file >> recordSize;
    file.ignore(); // Skip the delimiter

    // Move to the end of the record and append * overwrite the last char with *
    file.seekp(offset + recordSize , ios::beg);
    file.put('*'); // Mark the record as deleted
    availList.push_back(offset);

}

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
        deleteRecord(file, offset, doctorAvailList);
        p.removePrimaryIndex(primaryIndex,doctorID);// Remove from primary index
        cout << "Doctor record deleted successfully.\n";

        p.savePrimaryIndexToFile(primaryIndex, "PrimaryIndexOnDocID.txt");
    }
    void printDoctorInfo(const string& doctorID) {
        // Check if the Doctor ID exists in the primary index
        if (primaryIndex.find(doctorID) == primaryIndex.end()) {
            cout << "Doctor ID not found.\n";
            return;
        }
        useprimaryIndex p ;
        int offset = p.binarySearchPrimaryIndex(primaryIndex,doctorID);
        // Open the file to read the doctor information
        fstream file("Doctor.txt", ios::in);
        if (!file) {
            cerr << "Error opening file to read doctor information.\n";
            return;
        }
        // Read the record using readRecord function
        Record record = readRecord(file, offset);
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
        deleteRecord(file, offset, appointmentAvailList);
        p.removePrimaryIndex(primaryIndexOnAppoint,AppID);// Remove from primary index
        cout << "Appointment record deleted successfully.\n";

        p.savePrimaryIndexToFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
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
        // Read the record using readRecord function
        Record record = readRecord(file, offset);
        file.close();
        cout << "Appointment Information:\n";
        cout << "Appointment ID: " << record.s1 << "\n";
        cout << "Appointment Date: " << record.s2 << "\n";
        cout << "Doctor ID: " << record.s3 << "\n";
    }
};
// void printAvailList(const vector<int>& availList) {
//     if (availList.empty()) {
//         cout << "Empty\n";
//         return;
//     }
//     for (int offset : availList) {
//         cout << offset << " ";
//     }
//     cout << endl;
// }

int main() {
    useprimaryIndex p;
    p.loadPrimaryIndexFromFile(primaryIndex, "PrimaryIndexOnDocID.txt");
    p.loadPrimaryIndexFromFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
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
        //printAvailList(doctorAvailList ); to debug the current content of availList
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


