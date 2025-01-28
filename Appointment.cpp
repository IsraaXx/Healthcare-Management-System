#include "Doctor.h"
#include "Index.h"
#include "Record.h"
#include "Appointment.h"

void Appointment:: addAppointment(const string &appointmentID, const string &appointmentDate, const string &doctorID,
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


void Appointment:: deleteAppointment(const string &appointmentID, const string &doctorID) {
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


void Appointment::updateAppointmentDate(const string &appID, const string &newAppDate) {
    // Check if the appointment exists
    if (primaryIndexOnAppoint.find(appID) == primaryIndexOnAppoint.end()) {
        cout << "Appointment ID not found. Update failed.\n";
        return;
    }

    if (newAppDate.length() > 30) {
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
        // Pad with spaces if the new date is shorter
        string paddedDate = newAppDate;
        paddedDate.append(oldAppDate.length() - newAppDate.length(), ' ');
        currentRecord.s2 = paddedDate;

        // Save the updated record back to the file
        string updatedRecord = currentRecord.s1 + "|" + currentRecord.s2 + "|" + currentRecord.s3 + "|";
        file.seekp(offset, ios::beg);
        file << updatedRecord.size() << updatedRecord;

        cout << "Appointment Date updated successfully from " << oldAppDate << " to " << newAppDate << ".\n";
    } else {
        // If the new date is longer, delete and re-add the record
        file.close();
        deleteAppointment(appID, currentRecord.s3);
        addAppointment(appID, newAppDate, currentRecord.s3, primaryIndexOnAppoint);

        // Avoid redundant index manipulation (handled by addAppointment)
        cout << "Appointment Date updated by deleting and adding a new record.\n";
        return; // Exit early as `addAppointment` already handles index updates
    }

    file.close();
}

void Appointment:: printAppointmentInfo(const string& AppID) {
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
