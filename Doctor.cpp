#include "Doctor.h"
#include "Index.h"
#include "Record.h"
#include "Appointment.h"
void Doctor:: addDoctor(const string& doctorID, const string& name, const string& address,map<string, int>& primaryIndex) {
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
void Doctor:: deleteDoctor(const string &doctorID,const string& doctorName) {
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
//to delete from secondary index on name omnly not app secindex as we use it in updata on doctor name because it the same docID
void Doctor:: deleteDoctor2(const string &doctorID,const string& doctorName) {
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
}
void Doctor:: printDoctorInfo(const string& doctorID) {
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
void Doctor:: updateDoctorName(const string& doctorID, const string& newDoctorName) {
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
        deleteDoctor2(doctorID, oldDoctorName);

        addDoctor(doctorID, newDoctorName, currentRecord.s3, primaryIndex);

        cout << "Doctor name updated successfully by deleting and adding a new record.\n";
        return;
    }

    file.close();

    // Update the secondary index
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
