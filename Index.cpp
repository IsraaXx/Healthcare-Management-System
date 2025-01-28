#include "Index.h"
#include "Record.h"
#include "Doctor.h"
#include "Appointment.h"
void useSecondaryIndex:: addSecondaryIndex(const string &secondaryKey,const string &primaryKey) { //take seckey and pk to put in inverted list
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
vector<string> useSecondaryIndex ::  searchByName(const string &secondaryKey) const {
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
bool useSecondaryIndex:: removeSecondaryIndex(const string &secondaryKey, const string &primaryKey) {
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
void useSecondaryIndex:: saveSecondaryIndexToFile(const string &filename) const {
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

void useSecondaryIndex:: loadSecondaryIndexFromFile(const string &filename) {
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

// Function to add a record to the secondary index
void UseSecondaryIndexonAppoint:: addSecondaryIndex(const string &doctorID, const string &appointmentID) {
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
vector<string> UseSecondaryIndexonAppoint:: searchByDoctorID(const string &doctorID) const {
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
bool UseSecondaryIndexonAppoint:: removeSecondaryIndex(const string &doctorID, const string &appointmentID) {
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
void UseSecondaryIndexonAppoint:: saveSecondaryIndexToFile(const string &filename) const {
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

void UseSecondaryIndexonAppoint:: loadSecondaryIndexFromFile(const string &filename) {
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

void useprimaryIndex:: addPrimaryIndex(map<string, int> &primaryIndexm, const string &key, int offset) {
    primaryIndexm[key] = offset;
}

void useprimaryIndex:: removePrimaryIndex(map<string, int> &primaryIndexm, const string &key) {
    primaryIndexm.erase(key);
}

int useprimaryIndex:: binarySearchPrimaryIndex(const map<string, int> &primaryIndexm, const string &key) {
    auto it = primaryIndexm.find(key); // Binary search happens internally in the map
    if (it != primaryIndexm.end()) {
        return it->second; // Return offset if key is found
    }
    return -1; // Return -1 if not found
}

void useprimaryIndex:: savePrimaryIndexToFile(const map<string, int> &primaryIndexm, const string &filename) {
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

void useprimaryIndex:: loadPrimaryIndexFromFile(map<string, int> &primaryIndex, const string &filename) {
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
void useprimaryIndex:: updatePrimaryIndex(map<string, int> &primaryIndexm, const string &key, int newOffset) {
    auto it = primaryIndexm.find(key);
    if (it != primaryIndexm.end()) {
        it->second = newOffset; // Update the offset for the key
        cout << "Primary index updated for key: " << key << " with new offset: " << newOffset << endl;
    } else {
        cerr << "Key not found in primary index: " << key << ". Update failed.\n";
    }
}
