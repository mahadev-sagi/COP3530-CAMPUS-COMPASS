#pragma once
#include <string>
#include<map>
#include<vector>
#include<cmath>
#include<algorithm>
#include<iomanip>
#include<unordered_map>
#include<unordered_set>
#include<stack>
#include<set>

using namespace std;
struct Student {
    string name;
    int id;
    unordered_set<string> classes;
    int residenceID;
};
struct Class {
    string classCode;
    int locationID;
    string startTime;
    string endTime;
};
class CampusCompass {
private:
    unordered_map<int,string> locations;
    unordered_map<int,vector<pair<int,int>>> graph;
    unordered_map<int, Student> students;
    unordered_map<string, Class> classes;
    set<pair<int,int>> closedEdges;

    // Think about what member variables you need to initialize
    // perhaps some graph representation?
public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);
};
