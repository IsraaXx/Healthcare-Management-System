#include <iostream>
#include <fstream>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include <limits> // For numeric_limits
using namespace std;

struct Doctor {
    char doctorID[15];
    char doctorName[30];
    char address[30];
    void addDoctor(const string& doctorID, const string& name, const string& address,map<string, int>& primaryIndex) ;
    void deleteDoctor(const string &doctorID,const string& doctorName);
    //to delete from secondary index on name omnly not app secindex as we use it in updata on doctor name because it the same docID
    void deleteDoctor2(const string &doctorID,const string& doctorName) ;
    void printDoctorInfo(const string& doctorID);
    void updateDoctorName(const string& doctorID, const string& newDoctorName) ;
};


#ifndef HCMS2_DOCTOR_H
#define HCMS2_DOCTOR_H

#endif //HCMS2_DOCTOR_H
