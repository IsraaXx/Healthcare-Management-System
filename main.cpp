#include <iostream>
#include <fstream>
//#include <bits/stdc++.h>
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
void toLowerCase(string &str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower);
}
string trim(const std::string& str) {
    // Find the first non-space character
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";  // No non-space characters found
    }

    // Find the last non-space character
    size_t last = str.find_last_not_of(" \t\n\r");

    // Return the substring from first to last
    return str.substr(first, last - first + 1);
}
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
class useSecondaryIndex {// Class to manage the secondary index on doctor name
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
// Structure for an inverted list node that contains the primary key (Appointment ID) and a pointer to the next node in the list
struct InvertedListNodeA {
    string appointmentID;  // Primary key (Appointment ID)
    int nextPointer;

    InvertedListNodeA(const string &id, int next = -1) : appointmentID(id), nextPointer(next) {}
};

class UseSecondaryIndexonAppoint {
private:
    map<string, int> secondaryIndex;  // Maps Doctor ID to the head of the linked list (first node)
    vector<InvertedListNodeA> invertedList;  // Simulates the inverted list

public:
    // Function to add a record to the secondary index
    void addSecondaryIndex(const string &doctorID, const string &appointmentID) {
        int newNodeIndex = invertedList.size();
        invertedList.emplace_back(appointmentID, -1);

        if (secondaryIndex.find(doctorID) == secondaryIndex.end()) {
            secondaryIndex[doctorID] = newNodeIndex;
        } else {
            int currentIndex = secondaryIndex[doctorID];
            while (invertedList[currentIndex].nextPointer != -1) {
                currentIndex = invertedList[currentIndex].nextPointer;
            }
            invertedList[currentIndex].nextPointer = newNodeIndex;
        }

        cout << "Added record for " << doctorID << " with pointer to primary key in inverted list " << appointmentID << endl;
    }

    // Function to search for appointment IDs by Doctor ID
    vector<string> searchByDoctorID(const string &doctorID) const {
        vector<string> results;
        trim(doctorID);  // Remove leading and trailing spaces
        if (secondaryIndex.find(doctorID) == secondaryIndex.end()) {
            return results;
        }

        int currentIndex = secondaryIndex.at(doctorID);
        while (currentIndex != -1) {
            results.push_back(invertedList[currentIndex].appointmentID);
            currentIndex = invertedList[currentIndex].nextPointer;
        }
        return results;
    }

    // Function to delete an appointment ID from the Doctor ID's linked list
    bool removeSecondaryIndex(const string &doctorID, const string &appointmentID) {
        if (secondaryIndex.find(doctorID) == secondaryIndex.end()) {
            return false;
        }

        int currentIndex = secondaryIndex[doctorID];
        int prevIndex = -1;

        while (currentIndex != -1) {
            if (invertedList[currentIndex].appointmentID == appointmentID) {
                if (prevIndex == -1) {
                    secondaryIndex[doctorID] = invertedList[currentIndex].nextPointer;
                } else {
                    invertedList[prevIndex].nextPointer = invertedList[currentIndex].nextPointer;
                }

                invertedList[currentIndex]=(InvertedListNodeA("#", -1));

                if (secondaryIndex[doctorID] == -1) {
                    secondaryIndex[doctorID] = -1;
                }

                return true;
            }
            prevIndex = currentIndex;
            currentIndex = invertedList[currentIndex].nextPointer;
        }
        return false;
    }

    // Function to save the index and list into a file
    void saveSecondaryIndexToFile(const string &filename) const {
        ofstream outFile(filename);
        if (!outFile.is_open()) {
            cerr << "Error: Unable to open file for writing: " << filename << endl;
            return;
        }

        outFile << "Secondary Index:\n";
        for (const auto &entry: secondaryIndex) {
            if (entry.second >= 0 && entry.second < static_cast<int>(invertedList.size())) {
                outFile << entry.first << " -> " << entry.second << "\n";
            } else if (entry.second == -1) {
                outFile << entry.first << " -> -1\n";
            }
        }

        outFile << "\nInverted List:\n";
        for (size_t i = 0; i < invertedList.size(); ++i) {
            const auto &node = invertedList[i];
            if (node.appointmentID == "#") {
                outFile << i << ": (#, #)\n";
            } else {
                outFile << i << ": (" << node.appointmentID << ", " << node.nextPointer << ")\n";
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
                    size_t arrowPos = line.find(" -> ");
                    if (arrowPos != string::npos) {
                        string key = line.substr(0, arrowPos);
                        try {
                            int headIndex = stoi(line.substr(arrowPos + 4));
                            secondaryIndex[key] = headIndex;
                        } catch (const std::exception &e) {
                            cerr << "Error: Invalid format in secondary index: " << line << endl;
                        }
                    }
                } else if (section == "InvertedList") {
                    size_t colonPos = line.find(": ");
                    if (colonPos != string::npos) {
                        string rest = line.substr(colonPos + 2);
                        size_t commaPos = rest.find(", ");
                        if (rest.front() == '(' && rest.back() == ')' && commaPos != string::npos) {
                            string appointmentID = rest.substr(1, commaPos - 1);
                            string nextPointerStr = rest.substr(commaPos + 2, rest.length() - commaPos - 3);

                            try {
                                int nextPointer = (nextPointerStr == "#") ? -1 : stoi(nextPointerStr);
                                invertedList.emplace_back(appointmentID, nextPointer);
                            } catch (const std::exception &e) {
                                cerr << "Error: Invalid format in inverted list: " << rest << endl;
                            }
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
    void updatePrimaryIndex(map<string, int> &primaryIndexm, const string &key, int newOffset) {
        auto it = primaryIndexm.find(key);
        if (it != primaryIndexm.end()) {
            it->second = newOffset; // Update the offset for the key
            cout << "Primary index updated for key: " << key << " with new offset: " << newOffset << endl;
        } else {
            cerr << "Key not found in primary index: " << key << ". Update failed.\n";
        }
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
        UseSecondaryIndexonAppoint secApp;
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
        secApp.loadSecondaryIndexFromFile("SecondaryIndexOnDocID.txt");
        vector<string> v;
       v=secApp.searchByDoctorID(doctorID);
        for (int i = 0; i < v.size(); ++i) {
            secApp.removeSecondaryIndex(doctorID,v[i]);
        }
        secApp.saveSecondaryIndexToFile("SecondaryIndexOnDocID.txt");
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
    void updateDoctorName(const string& doctorID, const string& newDoctorName) {
    if (primaryIndex.find(doctorID) == primaryIndex.end()) {
        cout << "Doctor ID not found. Update failed.\n";
        return;
    }

    if (newDoctorName.length() > 30) {
        cout << "Error: Doctor name cannot exceed 30 characters.\n";
        return;
    }

    fstream file("Doctor.txt", ios::in | ios::out);
    if (!file) {
        cerr << "Error opening file to update doctor information.\n";
        return;
    }

    useprimaryIndex p;
    int offset = p.binarySearchPrimaryIndex(primaryIndex, doctorID);
    Record record;
    Record currentRecord = record.readRecord(file, offset);
    string oldDoctorName = currentRecord.s2;

    if (newDoctorName.length() <= oldDoctorName.length()) {
        // Pad with spaces if the new name is shorter or equal in length best fit strategy
        string paddedName = newDoctorName;
        paddedName.append(oldDoctorName.length() - newDoctorName.length(), ' ');
        currentRecord.s2 = paddedName;

        // Save the updated record back to the file
        string updatedRecord = currentRecord.s1 + "|" + currentRecord.s2 + "|" + currentRecord.s3 + "|";
        file.seekp(offset, ios::beg);
        file << updatedRecord.size() << updatedRecord;

        cout << "Doctor name updated successfully from " << oldDoctorName << " to " << newDoctorName << ".\n";

    } else {
        // If the new name is longer than the old one, remove and re-add the record
        file.close();
        deleteDoctor(doctorID, oldDoctorName);
        addDoctor(doctorID, newDoctorName, currentRecord.s3, primaryIndex);
        cout << "Doctor name updated successfully by deleting and adding a new record.\n";
    }
    file.close();
    useSecondaryIndex secIdx;
    secIdx.loadSecondaryIndexFromFile("SecondaryIndexOnDrName.txt");
    // Remove the old name from the secondary index
    if (!secIdx.removeSecondaryIndex(oldDoctorName, doctorID)) {
        cout << "Old doctor name not found in secondary index.\n";
    }
    // Add the new name to the secondary index
    secIdx.addSecondaryIndex(newDoctorName, doctorID);
    // Save the updated secondary index
    secIdx.saveSecondaryIndexToFile("SecondaryIndexOnDrName.txt");
}
};
struct Appointment {
    char appointmentID[15];
    char appointmentDate[30];
    char doctorID[15];

    void addAppointment(const string &appointmentID, const string &appointmentDate, const string &doctorID,
                        map<string, int> &primaryIndexOnAppoint) {
        // Check if the Appointment ID already exists
        if (primaryIndexOnAppoint.find(appointmentID) != primaryIndexOnAppoint.end()) {
            cout << "Appointment ID already exists! Enter another ID.\n";
            return;
        }

        // Open the appointments file for appending
        fstream outfile("Appointment.txt", ios::in | ios::out|ios::app);
        if (!outfile.is_open()) {
            cerr << "Error: Unable to open file for appointments.\n";
            return;
        }
        Record r;
        int offset=r.writeRecord(outfile,appointmentID,appointmentDate,doctorID,primaryIndexOnAppoint,appointmentAvailList);
        useprimaryIndex primaryIndexManager;
        primaryIndexManager.loadPrimaryIndexFromFile(primaryIndexOnAppoint,"PrimaryIndexOnAppID.txt");
        primaryIndexManager.addPrimaryIndex(primaryIndexOnAppoint,appointmentID,offset);
        primaryIndexManager.savePrimaryIndexToFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
        r.saveAvailListToFile(appointmentAvailList,"appointmentAvailList.txt");
        UseSecondaryIndexonAppoint secIdx;
        secIdx.loadSecondaryIndexFromFile("SecondaryIndexOnDocID.txt");
        secIdx.addSecondaryIndex(doctorID, appointmentID);
        secIdx.saveSecondaryIndexToFile("SecondaryIndexOnDocID.txt");
        cout << "Appointment added successfully.\n";
        outfile.close();
        
    }


    void deleteAppointment(const string &appointmentID, const string &doctorID) {
        // Check if the appointment exists in the primary index
        if (primaryIndexOnAppoint.find(appointmentID) == primaryIndexOnAppoint.end()) {
            cout << "Appointment ID you want to delete does not exist. Deletion failed.\n";
            return;
        }

        fstream file("Appointment.txt", ios::in | ios::out);
        if (!file) {
            cerr << "Error opening file for deleting appointment.\n";
            return;
        }
        UseSecondaryIndexonAppoint secIdx;
        useprimaryIndex p;
        int offset = p.binarySearchPrimaryIndex(primaryIndexOnAppoint, appointmentID);
        Record r;
        Record record = r.readRecord(file, offset);
        r.deleteRecord(file, offset, appointmentAvailList);
        p.removePrimaryIndex(primaryIndexOnAppoint, appointmentID);
        secIdx.loadSecondaryIndexFromFile("SecondaryIndexOnDocID.txt");
        if (secIdx.removeSecondaryIndex(doctorID, appointmentID)) {
            cout << "Secondary index updated after appointment deletion.\n";
        } else {
            cout << "Appointment not found in secondary index.\n";
        }
        cout << "Appointment record deleted successfully.\n";

        // Save the updated indexes and availability list to their respective files
        p.savePrimaryIndexToFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
        secIdx.saveSecondaryIndexToFile("SecondaryIndexOnDocID.txt");
        r.saveAvailListToFile(appointmentAvailList, "appointmentAvailList.txt");
    }


    void updateAppointmentDate(const string &appID, const string &newAppDate) {
        // Check if the appointment exists
        if (primaryIndexOnAppoint.find(appID) == primaryIndexOnAppoint.end()) {
            cout << "Appointment ID not found. Update failed.\n";
            return;
        }
        if (newAppDate.length()>30){
            cout << "Error: Appointment Date cannot exceed 30 characters.\n";
            return;
        }
        // Open the file for reading and writing
        fstream file("Appointment.txt", ios::in | ios::out);
        if (!file) {
            cerr << "Error opening file to update appointment.\n";
            return;
        }

        // Locate the record offset using the primary index
        useprimaryIndex primaryIndexManager;
        int offset = primaryIndexManager.binarySearchPrimaryIndex(primaryIndexOnAppoint, appID);
        Record record;
        Record currentRecord = record.readRecord(file, offset);
        string oldAppDate = currentRecord.s2; // Old appointment date
        // Check if the new date size matches the old one
        if (newAppDate.length() <= oldAppDate.length()) {
             string paddedName = newAppDate;
        paddedName.append(oldAppDate.length() - newAppDate.length(), ' ');
        currentRecord.s2 = paddedName;

        // Save the updated record back to the file
        string updatedRecord = currentRecord.s1 + "|" + currentRecord.s2 + "|" + currentRecord.s3 + "|";
        file.seekp(offset, ios::beg);
        file << updatedRecord.size() << updatedRecord;

        cout << "Appointment Date updated successfully from " << oldAppDate << " to " << newAppDate << ".\n";
        } 
        else {
         file.close();
        deleteAppointment(appID,doctorID);
        addAppointment(appID, newAppDate, currentRecord.s3, primaryIndexOnAppoint);
        }
         file.close();

    // Update the secondary index
    UseSecondaryIndexonAppoint secIdx;
    secIdx.loadSecondaryIndexFromFile("SecondaryIndexOnDocID.txt");

    // Remove the old name from the secondary index
    if (!secIdx.removeSecondaryIndex(doctorID,appID)) {
        cout << "Old doctor name not found in secondary index.\n";
    }

    // Add the new name to the secondary index
    secIdx.addSecondaryIndex(doctorID,appID);
 secIdx.saveSecondaryIndexToFile("SecondaryIndexOnDocID.txt");
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

void processQuery() {
    Record r1;
    string query;
    cout << "Enter your query (e.g., SELECT * FROM doctor WHERE DoctorID='D123'):\n";
    cin.ignore(); // Clear the input buffer
    getline(cin, query);

    // Convert the query to lowercase for case-insensitive comparison
    string lowerQuery = query;
    toLowerCase(lowerQuery);

    // Check for the essential keywords
    size_t selectPos = lowerQuery.find("select");
    size_t fromPos = lowerQuery.find("from");
    size_t wherePos = lowerQuery.find("where");

    if (selectPos == string::npos || fromPos == string::npos || wherePos == string::npos) {
        cout << "Invalid query syntax. Please use the format: SELECT * FROM [table] WHERE [condition].\n";
        return;
    }

    // Extract the part after SELECT to check if it is * or specific fields
    size_t selectEnd = selectPos + 6; // Length of "select"
    string selectPart = lowerQuery.substr(selectEnd, fromPos - selectEnd); // Extract part between SELECT and FROM
    selectPart = trim(selectPart); // Remove extra spaces

    // Split the fields by commas and store them in a vector
    vector<string> fields;
    size_t pos = 0;
    while ((pos = selectPart.find(',')) != string::npos) {
        string field = selectPart.substr(0, pos);
        fields.push_back(trim(field)); // Trim spaces and push into the vector
        selectPart.erase(0, pos + 1);
    }
    // Add the last field (after the last comma)
    fields.push_back(trim(selectPart));

    // Extract table name
    size_t tableStart = fromPos + 5; // Skip "from "
    size_t tableEnd = lowerQuery.find(" ", tableStart);
    string table = lowerQuery.substr(tableStart, tableEnd - tableStart);

    // Extract condition
    size_t conditionStart = wherePos + 6; // Skip "where "
    string condition = query.substr(conditionStart);

    // Process the table
    if (table == "doctors") {
        if (condition.find("DoctorID=") != string::npos|| condition.find("Doctor ID=") != string::npos) {
            size_t pos = condition.find("=");
            if (pos != string::npos) {
                string doctorID = condition.substr(pos + 2, condition.length() - pos - 3); // Skip quotes
                if (primaryIndex.find(doctorID) != primaryIndex.end()) {
                    int offset = primaryIndex[doctorID];
                    fstream file("Doctor.txt", ios::in);
                    Record record = r1.readRecord(file, offset);
                    file.close();
                    cout << "Doctor Information:\n";
                    // Loop through fields and print requested information
                    for (const string& field : fields) {
                        if(field=="*" || field=="all" ){

                            cout << "Doctor ID: " << record.s1 << "\n";
                            cout << "Doctor Name: " << record.s2 << "\n";
                            cout << "Address: " << record.s3 << "\n";
                        }
                        else if ( field == "doctorid"||field == "doctor id" ) {
                            cout << "Doctor ID: " << record.s1 << "\n";
                        } else if (field == "doctorname" || field == "doctor name") {
                            cout << "Doctor Name: " << record.s2 << "\n";
                        } else if (field == "doctoraddress" || field == "doctor address") {
                            cout << "Address: " << record.s3 << "\n";
                        }
                    }
                } else {
                    cout << "No doctor found with ID: " << doctorID << endl;
                }
            } else {
                cout << "Invalid condition format. Use DoctorID='D123'.\n";
            }
        } else {
            cout << "Unsupported condition for table 'doctor'.\n";
        }
    }
    else if (table == "appointments") {
        if (condition.find("DoctorID=") != string::npos || condition.find("Doctor ID=") != string::npos) {
            size_t pos = condition.find("=");
            if (pos != string::npos) {
                string doctorID = condition.substr(pos + 2, condition.length() - pos - 3); // Remove quotes
                trim(doctorID); toLowerCase(doctorID); // Normalize

                UseSecondaryIndexonAppoint useIndex;
                useIndex.loadSecondaryIndexFromFile("SecondaryIndexOnDocID.txt"); // Load data
                cout << "Searching appointments for DoctorID: " << doctorID << endl;

                vector<string> appointmentIDs = useIndex.searchByDoctorID(doctorID);
                if (appointmentIDs.empty()) {
                    cout << "No appointments found for Doctor ID: " << doctorID << endl;
                    return;
                }

                // Access Appointment file
                fstream file("Appointment.txt", ios::in);
                if (!file.is_open()) {
                    cerr << "Error: Unable to open Appointment.txt file.\n";
                    return;
                }

                cout << "Appointments for Doctor ID: " << doctorID << endl;
                for (const string& appID : appointmentIDs) {
                    if (primaryIndexOnAppoint.find(appID) != primaryIndexOnAppoint.end()) {
                        int offset = primaryIndexOnAppoint[appID];
                        Record record = r1.readRecord(file, offset);
                        for (const string& field : fields) {
                            if(field=="*" || field=="all" ){
                                cout << "Appointment Information:\n";
                                cout << "Appointment ID: " << record.s1 << "\n";
                                cout << "Appointment Date: " << record.s2 << "\n";
                                cout << "Doctor ID: " << record.s3 << "\n";
                            }
                            else if (field == "appointmentid" ||field == "appointment id" ) {
                                cout << "Appointment ID: " << record.s1 << "\n";
                            } else if (field == "appointmentdate" || field == "appointment date") {
                                cout << "Appointment Date: " << record.s2 << "\n";
                            } else if (field == "doctorid" ||field=="doctor id") {
                                cout << "Doctor ID: " << record.s3 << "\n";
                            }
                        }
                    } else {
                        cout << "Error: Appointment ID " << appID << " not found in primary index.\n";
                    }
                }
                file.close();
            } else {
                cout << "Invalid condition format. Use DoctorID='D123'.\n";
            }
        } else {
            cout << "Unsupported condition for table 'appointment'.\n";
        }
    }
    else {
        cout << "Invalid table name: " << table << ".\n";
    }
}

int main() {

    useprimaryIndex p;
    UseSecondaryIndexonAppoint ss;
    Record r1;
    p.loadPrimaryIndexFromFile(primaryIndex, "PrimaryIndexOnDocID.txt");
    p.loadPrimaryIndexFromFile(primaryIndexOnAppoint, "PrimaryIndexOnAppID.txt");
    ss.loadSecondaryIndexFromFile("SecondaryIndexOnDocID.txt");
    r1.loadAvailListFromFile(doctorAvailList, "doctorAvailList.txt");
    r1.loadAvailListFromFile(appointmentAvailList, "appointmentAvailList.txt");
    while (true) {
        cout << "\nWelcome! to Healthcare management system\n";
        cout << "1. Add New Doctor\n";
        cout << "2. Add New Appointment\n";
        cout << "3. Update Doctor Name (Doctor ID)\n";
        cout << "4. Update Appointment Date (Appointment ID)\n";
        cout << "5. Delete Appointment (Appointment ID,Doctor ID)\n";
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
        if (choice == 2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
            Appointment App;
            cout << "Enter data of the Appointment :\n";
            cout << "appointmentID:";
            cin.getline(App.appointmentID, size(App.appointmentID));
            cout << "appointmentDate:";
            cin >> App.appointmentDate;
            cout << "doctorID:";
            cin.ignore();  // Clear the newline character left in the buffer
            cin.getline(App.doctorID, size(App.doctorID));
            App.addAppointment(App.appointmentID, App.appointmentDate, App.doctorID,primaryIndexOnAppoint);
        }
        if (choice == 3) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
            Doctor doc;
            string doctorID,doctorName;
            cout << "Enter the Doctor ID you want to update: ";
            cin >> doctorID;
            cout << "Enter the New Doctor Name:  ";
            cin.ignore(); // To clear any leftover newline in the buffer
            getline(cin, doctorName);
            doc.updateDoctorName(doctorID,doctorName);
        }
        if (choice == 4) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer
            string appID, newAppDate;
            cout << "Enter Appointment ID to update: ";
            getline(cin, appID);
            cout << "Enter new Appointment Date: ";
            getline(cin, newAppDate);
            Appointment App;
            App.updateAppointmentDate(appID, newAppDate);
        }

        if (choice == 5) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string AppID, DoctorID;
            cout << "Enter Appointment ID to delete: ";
            cin >> AppID;
            cout << "Enter Doctor ID associated with this appointment: ";
            cin >> DoctorID;
            Appointment app;  // Assuming Appointment class has a method deleteAppoint
            app.deleteAppointment(AppID, DoctorID) ;
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
        if (choice == 9) {
            processQuery();
        }
    }
}
