#include "CampusCompass.h"
#include<iostream>
#include<fstream>
#include<sstream>
#include <string>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize your object
}

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    // return boolean based on whether parsing was successful or not
    string line;
    ifstream classFile(classes_filepath);
    if (!classFile.is_open()) {
        cerr<<"Error opening file "<<classes_filepath<<endl;
        return false;
    }

    getline(classFile, line);
    while (getline(classFile,line)) {
        stringstream ss(line);
        string classCode, startTime, endTime, locationStr;
        getline(ss, classCode, ',');
        getline(ss,locationStr, ',');
        getline(ss, startTime, ',');
        getline(ss, endTime);
        int locationID = stoi(locationStr);
        Class newClass;
        newClass.classCode = classCode;
        newClass.startTime = startTime;
        newClass.endTime = endTime;
        newClass.locationID = locationID;
        classes[classCode] = newClass;
    }
    ifstream edgesFile(edges_filepath);
    if (!edgesFile.is_open()) {
        cerr<<"Error opening file "<<edges_filepath<<endl;
        return false;
    }
    while (getline(edgesFile,line)) {
        stringstream ss(line);
        string location1, location2, name1, name2, timeStr;
        getline(ss, location1, ',');
        getline(ss, location2, ',');
        getline(ss, name1, ',');
        getline(ss, name2, ',');
        getline(ss, timeStr);

        int loc1 = stoi(location1);
        int loc2 = stoi(location2);
        int time = stoi(timeStr);
        locations[loc1] = name1;
        locations[loc2] = name2;

        graph[loc1].push_back({loc2,time});
        graph[loc2].push_back({loc1,time});

    }


    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    // do whatever regex you need to parse validity
    // hint: return a boolean for validation when testing. For example:
    //bool is_valid = true; // replace with your actual validity checking
    stringstream ss(command);
    string Command;
    ss>>Command;
    if (Command == "insert") {

    }
    else if (Command == "remove") {
        int studentID;
        ss >> studentID;
        if (to_string(studentID).length() != 8) {
            cout <<"unsuccessful"<<endl;
            return false;
        }

        if (students.find(studentID) != students.end()) {
            students.erase(studentID);
            cout << "successful"<<endl;
            return true;
        }else {
            cout << "unsuccessful"<<endl;
            return false;
        }

    }
    else if (Command =="dropClass"){}

    return true;

}
