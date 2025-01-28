#include <iostream>
#include <fstream>
#include<map>
#include<vector>
#include<string>
#include<algorithm>
#include <limits> // For numeric_limits
using namespace std;


struct InvertedListNode {
    string primaryKey;  // The primary key associated with the node
    int nextPointer;         // The index of the next node (-1 if it's the last node)

    InvertedListNode(const std::string& pk, int next = -1)
            : primaryKey(pk), nextPointer(next) {}
};

struct InvertedListNodeA {
    string appointmentID;  // Primary key (Appointment ID)
    int nextPointer;

    InvertedListNodeA(const string &id, int next = -1) : appointmentID(id), nextPointer(next) {}
};
class useSecondaryIndex {// Class to manage the secondary index on doctor name
private:
    map<string, int> secondaryIndex; // Maps secondary key to the head of the linked list (first node)
    vector<InvertedListNode> invertedList;     // Simulates the inverted list
public:
    // Function to add a record to the secondary index
    void addSecondaryIndex(const string &secondaryKey,const string &primaryKey) ;
    // Function to search for primary keys by secondary key
    vector<string> searchByName(const string &secondaryKey) const;
    // Function to delete a primary key from the secondary key's linked list
    bool removeSecondaryIndex(const string &secondaryKey, const string &primaryKey);
    // Function to save the index and list into a file
    void saveSecondaryIndexToFile(const string &filename) const ;

    void loadSecondaryIndexFromFile(const string &filename) ;

};
class UseSecondaryIndexonAppoint {
private:
    map<string, int> secondaryIndex;  // Maps Doctor ID to the head of the linked list (first node)
    vector<InvertedListNodeA> invertedList;  // Simulates the inverted list

public:
    // Function to add a record to the secondary index
    void addSecondaryIndex(const string &doctorID, const string &appointmentID);

    // Function to search for appointment IDs by Doctor ID
    vector<string> searchByDoctorID(const string &doctorID) const ;
    // Function to delete an appointment ID from the Doctor ID's linked list
    bool removeSecondaryIndex(const string &doctorID, const string &appointmentID) ;

    // Function to save the index and list into a file
    void saveSecondaryIndexToFile(const string &filename) const ;

    void loadSecondaryIndexFromFile(const string &filename) ;
};
struct useprimaryIndex {
    void addPrimaryIndex(map<string, int> &primaryIndexm, const string &key, int offset) ;
    void removePrimaryIndex(map<string, int> &primaryIndexm, const string &key);

    int binarySearchPrimaryIndex(const map<string, int> &primaryIndexm, const string &key);

    void savePrimaryIndexToFile(const map<string, int> &primaryIndexm, const string &filename) ;

    void loadPrimaryIndexFromFile(map<string, int> &primaryIndex, const string &filename) ;
    void updatePrimaryIndex(map<string, int> &primaryIndexm, const string &key, int newOffset);
};

#ifndef HCMS2_INDEX_H
#define HCMS2_INDEX_H

#endif //HCMS2_INDEX_H
