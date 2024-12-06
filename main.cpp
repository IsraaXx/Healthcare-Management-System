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

int writeRecord(fstream &file,  const string &field1, const string &field2, const string &field3,
                     map<string, int> &anyprimaryIndex, vector<pair<int, int>> &availList) {
        // Calculate the record size
        string record = field1 + "|" + field2 + "|" + field3 + "|";
       // int recordSize = record.size();
       int recordSize = record.size(); // Add 1 for the newline character
        int offset = -1; // Variable to hold where the record will be written
        // Check availability list for best-fit space
        for (auto it = availList.begin(); it != availList.end(); ++it) {
            if (it->first >= recordSize) { // Check if space is sufficient
                 offset = it->second; // Use this offset
                availList.erase(it); // Remove this entry from availList
                break;
            }
        }
        if (offset == -1) { // No suitable space found in availList
            file.seekp(0, ios::end); // Move to end of file
            offset = file.tellp();
        }
        file.seekp(offset, ios::beg);  // Write the record to the file
        file << recordSize << record;
        return offset;   
}
};

struct InvertedListNode {
    string primaryKey;  // The primary key associated with the node
    int nextPointer;         // The index of the next node (-1 if it's the last node)

    InvertedListNode(const std::string& pk, int next = -1)
            : primaryKey(pk), nextPointer(next) {}
};
class useSecondaryIndex {// Class to manage the secondary index
private:
    map<string, int> secondaryIndex; // Maps secondary key to the head of the linked list (first node)
    vector<InvertedListNode> invertedList;     // Simulates the inverted list

public:
    // Function to add a record to the secondary index
    void addSecondaryIndex(const string &secondaryKey,const string &primaryKey) { //take seckey and pk to put in inverted list
        int newNodeIndex = invertedList.size(); // Create a new node for the inverted list
        invertedList.emplace_back(primaryKey, -1); // New node points to -1 initially (end of the list)
        // If the secondary key doesn't exist, add it with the new node as the head
        if (secondaryIndex.find(secondaryKey) == secondaryIndex.end()) {
            secondaryIndex[secondaryKey] = newNodeIndex;
        } else {
            // Otherwise, find the last node in the linked list and append the new node
            int currentIndex = secondaryIndex[secondaryKey];
            while (invertedList[currentIndex].nextPointer != -1) {
                currentIndex = invertedList[currentIndex].nextPointer;
            }
            invertedList[currentIndex].nextPointer = newNodeIndex; // Update the last node's next pointer
        }

        cout << "Added record for " << secondaryKey << " with pointer to primary key in inverted list " << primaryKey << endl;
    }

    // Function to search for primary keys by secondary key
    vector<string> searchByName(const string &secondaryKey) const {
        vector<string> results;
        if (secondaryIndex.find(secondaryKey) == secondaryIndex.end()) {
            return results; // Return empty if the secondary key is not found
        }

        int currentIndex = secondaryIndex.at(secondaryKey);
        while (currentIndex != -1) {
            results.push_back(invertedList[currentIndex].primaryKey);
            currentIndex = invertedList[currentIndex].nextPointer; // Move to the next node
        }
        return results;
    }
   
    // Function to delete a primary key from the secondary key's linked list
    bool removeSecondaryIndex(const string &secondaryKey, const string &primaryKey) {
         if (secondaryIndex.find(secondaryKey) == secondaryIndex.end()) {
            return false; // Secondary key not found
        }

        int currentIndex = secondaryIndex[secondaryKey];
        int prevIndex = -1;

        while (currentIndex != -1) {
            if (invertedList[currentIndex].primaryKey == primaryKey) {
                // If it's the head node, update the secondary index
                if (prevIndex == -1) {
                    secondaryIndex[secondaryKey] = invertedList[currentIndex].nextPointer;
                } else { // Otherwise, update previous node to skip the current node
                    invertedList[prevIndex].nextPointer = invertedList[currentIndex].nextPointer;
                }

                // Mark the current node as deleted (reset primary key to "#")
                invertedList[currentIndex] = InvertedListNode("#", -1);

                // Check if the list for this secondary key is now empty (i.e., no valid nodes)
                if (secondaryIndex[secondaryKey] == -1) {
                    // Instead of removing the secondary key, mark it as invalid
                    secondaryIndex[secondaryKey] = -1;
                }

                return true; // Successfully deleted
            }
            prevIndex = currentIndex;
            currentIndex = invertedList[currentIndex].nextPointer; // Move to the next node
        }
        return false; // If the primary key is not found in the list
    }

    // Function to save the index and list into a file
    void saveSecondaryIndexToFile(const string &filename) const {
        ofstream outFile(filename, ios::out);
        if (!outFile.is_open()) {
            cerr << "Error: Unable to open file for writing: " << filename << endl;
            return;
        }

        // Write the secondary index
        outFile << "Secondary Index:\n";
        for (const auto &entry: secondaryIndex) {
            if (entry.second >= 0 && entry.second < static_cast<int>(invertedList.size())) {
                outFile << entry.first << " -> " << entry.second << "\n";
            } else if (entry.second == -1) {
                // Handle case where secondary key has been deleted and marked as -1
                outFile << entry.first << " -> -1\n";
            }
        }

        // Write the inverted list
        outFile << "\nInverted List:\n";
        for (size_t i = 0; i < invertedList.size(); ++i) {
            const auto &node = invertedList[i];
            if (node.primaryKey == "#") {
                outFile << i << ": (#, #)\n"; // Mark deleted nodes as (#, #)
            } else {
                outFile << i << ": (" << node.primaryKey << ", " << node.nextPointer << ")\n";
            }
        }

        outFile.close();
        cout << "Secondary index saved successfully to " << filename << endl;
    }

    void loadSecondaryIndexFromFile(const string &filename) {
        ifstream inFile(filename);
        if (!inFile.is_open()) {
            cerr << "Error: Unable to open file for reading: " << filename << endl;
            return;
        }
         secondaryIndex.clear();
        invertedList.clear();

        string line, section;
        while (getline(inFile, line)) {
            if (line == "Secondary Index:") {
                section = "SecondaryIndex";
            } else if (line == "Inverted List:") {
                section = "InvertedList";
            } else if (!line.empty()) {
                if (section == "SecondaryIndex") {
                    // Parse secondary index entries
                    size_t arrowPos = line.find(" -> ");
                    if (arrowPos != string::npos) {
                        string key = line.substr(0, arrowPos);
                        try {
                            int headIndex = stoi(line.substr(arrowPos + 4));
                            secondaryIndex[key] = headIndex;
                        } catch (const std::invalid_argument &e) {
                            cerr << "Error: Invalid format for headIndex: " << line << endl;
                        } catch (const std::out_of_range &e) {
                            cerr << "Error: headIndex out of range: " << line << endl;
                        }
                    }
                } else if (section == "InvertedList") {
                    // Parse inverted list entries
                    size_t colonPos = line.find(": ");
                    if (colonPos != string::npos) {
                        string rest = line.substr(colonPos + 2);
                        size_t commaPos = rest.find(", ");
                        if (rest.front() == '(' && rest.back() == ')' && commaPos != string::npos) {
                            string primaryKey = rest.substr(1, commaPos - 1); // Remove parentheses
                            string nextPointerStr = rest.substr(commaPos + 2, rest.length() - commaPos - 3);

                            try {
                                // Replace any invalid nextPointer with -1
                                int nextPointer = (nextPointerStr == "#") ? -1 : stoi(nextPointerStr);
                                invertedList.emplace_back(primaryKey, nextPointer);
                            } catch (const std::invalid_argument &e) {
                                cerr << "Error: Invalid format for nextPointer: " << nextPointerStr << endl;
                            } catch (const std::out_of_range &e) {
                                cerr << "Error: nextPointer out of range: " << nextPointerStr << endl;
                            }
                        } else {
                            cerr << "Error: Malformed inverted list entry: " << rest << endl;
                        }
                    }
                }
            }
        }

        inFile.close();
        cout << "Secondary index loaded from" << filename << endl;
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
        outfile.open("Doctor.txt", ios::in|ios::out);
        Record r;
        int offset=r.writeRecord(outfile, doctorID, name, address, primaryIndex, doctorAvailList);
        useprimaryIndex p;
        p.addPrimaryIndex(primaryIndex,doctorID,offset);
        p.savePrimaryIndexToFile(primaryIndex, "PrimaryIndexOnDocID.txt");
        r.saveAvailListToFile(doctorAvailList,"doctorAvailList.txt");
         useSecondaryIndex secIdx;
        secIdx.loadSecondaryIndexFromFile("SecondaryIndexOnDrName.txt");
        secIdx.addSecondaryIndex(name, doctorID);
        secIdx.saveSecondaryIndexToFile("SecondaryIndexOnDrName.txt");
        cout << "Doctor added successfully.\n";
        outfile.close();
    }
    void deleteDoctor(const string &doctorID,const string& doctorName) {
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
        useSecondaryIndex secIdx;
        int offset = p.binarySearchPrimaryIndex(primaryIndex,doctorID);
        Record r1;
          r1.deleteRecord(file, offset, doctorAvailList);
        p.removePrimaryIndex(primaryIndex,doctorID);
        secIdx.loadSecondaryIndexFromFile("SecondaryIndexOnDrName.txt");
        if(secIdx.removeSecondaryIndex(doctorName,doctorID))
        {
            cout << "Secondary index updated after doctor deletion.\n";
        }
        else {
            cout << "Doctor not found in secondary index.\n";
        }
        cout << "Doctor record deleted successfully.\n";
        p.savePrimaryIndexToFile(primaryIndex, "PrimaryIndexOnDocID.txt");
        secIdx.saveSecondaryIndexToFile("SecondaryIndexOnDrName.txt");
        r1.saveAvailListToFile(doctorAvailList, "doctorAvailList.txt");
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
        r2.saveAvailListToFile(appointmentAvailList, "appointmentAvailList.txt");
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
        cout << "6. Delete Doctor (Doctor ID,Doctor Name)\n";
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
            string doctorID,doctorName;
            cout << "Enter the Doctor ID to delete: ";
            cin >> doctorID;
            cout << "Enter the Doctor Name associated with the ID: ";
            cin.ignore(); // To clear any leftover newline in the buffer
            getline(cin, doctorName);
            Doctor doc; 
            doc.deleteDoctor(doctorID,doctorName);
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
