#include "Doctor.h"
#include <iostream>
#include "Appointment.h"
#include "Index.h"
#include "Record.h"
 map<string, int> primaryIndex;
 map<string, int> primaryIndexOnAppoint;
 vector<pair<int,int>> doctorAvailList;
 vector<pair<int,int>> appointmentAvailList;
using namespace std;

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
