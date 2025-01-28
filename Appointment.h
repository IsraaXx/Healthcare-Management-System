#include <iostream>
#include <fstream>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include <limits> // For numeric_limits
using namespace std;

struct Appointment {
    char appointmentID[15];
    char appointmentDate[30];
    char doctorID[15];

    void addAppointment(const string &appointmentID, const string &appointmentDate, const string &doctorID,
                        map<string, int> &primaryIndexOnAppoint) ;
    void deleteAppointment(const string &appointmentID, const string &doctorID) ;
    void updateAppointmentDate(const string &appID, const string &newAppDate);
    void printAppointmentInfo(const string& AppID) ;
};
#ifndef HCMS2_APPOINTMENT_H
#define HCMS2_APPOINTMENT_H

#endif //HCMS2_APPOINTMENT_H
