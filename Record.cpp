#include "Record.h"
#include "Index.h"
#include "Doctor.h"
#include "Appointment.h"
Record Record:: readRecord(fstream &file, int offset) {
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
void Record:: deleteRecord(fstream &file, int offset, vector<pair<int,int>> &availList) {
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
void Record:: saveAvailListToFile(const vector<pair<int, int>> &availList, const string &filename) {
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
void Record:: loadAvailListFromFile(vector<pair<int, int>> &availList, const string &filename) {
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

int Record:: writeRecord(fstream &file,  const string &field1, const string &field2, const string &field3,
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

void toLowerCase(string &str) {
    transform(str.begin(), str.end(), str.begin(), ::tolower); }

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
