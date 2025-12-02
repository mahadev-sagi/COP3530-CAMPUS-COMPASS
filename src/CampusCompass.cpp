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
        string name;
        int studentID, residenceID, numberofClasses;
        char quote;
        ss >> quote;
        getline(ss, name, '"');
        ss>> studentID>>residenceID>>numberofClasses;
        if (to_string(studentID).length() != 8) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (students.find(studentID) != students.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        for (char c: name) {
            if (!isalpha(c) && c != ' ') {
                cout<<"unsuccessful"<<endl;
                return false;
            }
        }
        if (numberofClasses < 1 || numberofClasses > 6) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        vector<string> classesVector;
        for (auto i = 0; i < numberofClasses; i++) {
            string classCode;
            ss>> classCode;
            if (classCode.length() != 7) {
                cout<<"unsuccessful"<<endl;
                return false;
            }
            for (auto j = 0; j < 3; j++) {
                if (!isupper(classCode[j])) {
                    cout<<"unsuccessful"<<endl;
                    return false;
                }
            }
            for (auto j = 3; j < 7; j++) {
                if (!isdigit(classCode[j])) {
                    cout<<"unsuccessful"<<endl;
                    return false;
                }
            }
            if (classes.find(classCode) == classes.end()) {
                cout<<"unsuccessful"<<endl;
                return false;
            }
            classesVector.push_back(classCode);
        }
       Student student;
        student.name = name;
        student.id = studentID;
        student.residenceID = residenceID;
        for (string& classCode : classesVector) {
            student.classes.insert(classCode);
        }
        students[studentID] = student;
        cout<<"successful"<<endl;
        return true;



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
    else if (Command =="dropClass") {
        int studentID;
        string classCode;
        ss>>studentID>>classCode;
        if (to_string(studentID).length() != 8) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (students.find(studentID) == students.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (classes.find(classCode) == classes.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (students[studentID].classes.find(classCode) == students[studentID].classes.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        students[studentID].classes.erase(classCode);
        if (students[studentID].classes.empty()) {
            students.erase(studentID);
        }
        cout<<"successful"<<endl;
        return true;
    }
    else if (Command == "replaceClass") {
        int studentID;
        string classCode1, classCode2;
        ss>>studentID>>classCode1>>classCode2;
        if (to_string(studentID).length() != 8) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (students.find(studentID) == students.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (classes.find(classCode2) == classes.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (students[studentID].classes.find(classCode1) == students[studentID].classes.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        if (students[studentID].classes.find(classCode2) != students[studentID].classes.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        students[studentID].classes.erase(classCode1);
        students[studentID].classes.insert(classCode2);
        cout<<"successful"<<endl;
        return true;
    }
    else if (Command == "removeClass") {
        string classCode;
        ss >> classCode;
        int count = 0;
        auto it = students.begin();
        while (it != students.end()) {
            if (it->second.classes.count(classCode)) {
                it->second.classes.erase(classCode);
                count++;
                if (it->second.classes.empty()) {
                    it = students.erase(it);
                }else {
                    it++;
                }
            }
            cout<<count<<endl;
            return true;
        }
        cout<<count<<endl;
        return true;
    }
    else if (Command == "toggleEdgesClosure") {
        int n;
        ss >> n;
        for (int i = 0; i < n; i ++) {
            int locationX, locationY;
            ss>> locationX >> locationY;
            if (closedEdges.find({locationX, locationY}) != closedEdges.end()) {
                closedEdges.erase({locationX, locationY});
                closedEdges.erase({locationY, locationX});
            }else {
                closedEdges.insert({locationX, locationY});
                closedEdges.insert({locationY, locationX});
            }
        }
        cout<<"successful"<<endl;
        return true;
    }
    else if (Command == "checkEdgeStatus") {
        int id1, id2;
        ss >> id1 >> id2;
        bool edgeStatus = false;
        if (closedEdges.find({id1,id2}) != closedEdges.end() || closedEdges.find({id2,id1}) != closedEdges.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
    }



}
