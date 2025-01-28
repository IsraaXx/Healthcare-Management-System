#include <iostream>
#include <fstream>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include <limits> // For numeric_limits
using namespace std;
extern map<string, int> primaryIndex;
extern map<string, int> primaryIndexOnAppoint;
extern vector<pair<int,int>> doctorAvailList;
extern vector<pair<int,int>> appointmentAvailList;

void toLowerCase(string &str);
string trim(const std::string& str);
void processQuery() ;

struct Record{  //to implement read and write and delete record
    string s1;
    string s2;
    string s3;
    Record readRecord(fstream &file, int offset) ;
    void deleteRecord(fstream &file, int offset, vector<pair<int,int>> &availList);
    void saveAvailListToFile(const vector<pair<int, int>> &availList, const string &filename);
    void loadAvailListFromFile(vector<pair<int, int>> &availList, const string &filename) ;
    int writeRecord(fstream &file,  const string &field1, const string &field2, const string &field3,
                    map<string, int> &anyprimaryIndex, vector<pair<int, int>> &availList) ;



};

#ifndef HCMS2_RECORD_H
#define HCMS2_RECORD_H

#endif //HCMS2_RECORD_H
