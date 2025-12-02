#include "CampusCompass.h"
#include<iostream>
#include<limits>
#include<fstream>
#include<sstream>
#include <string>
#include<queue>
#include<stack>
#include<algorithm>
#include<set>
#include<unordered_set>
#include<unordered_map>

using namespace std;

CampusCompass::CampusCompass() {
    // initialize your object
}
unordered_map<int, int> CampusCompass::shortestPathsFrom(int source) {
    unordered_map<int, int> shortestPaths;
    priority_queue<
        pair<int,int>,
        vector<pair<int,int>>,
        greater<pair<int,int>>
    > pq;
    shortestPaths[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        pair<int,int> top = pq.top();
        pq.pop();

        int currentDist = top.first;
        int currentLocation = top.second;
        if (currentDist > shortestPaths[currentLocation]) {
            continue;
        }
        auto it = graph.find(currentLocation);
        if (it == graph.end()) {
            continue;
        }
        for (const auto &edge : it->second) {
            int neighborLocation = edge.first;
            int edgeTime = edge.second;
            if (closedEdges.count({currentLocation, neighborLocation})) {
                continue;
            }

            int newDist = currentDist + edgeTime;

            if (!shortestPaths.count(neighborLocation) || newDist < shortestPaths[neighborLocation]) {
                shortestPaths[neighborLocation] = newDist;
                pq.push({newDist, neighborLocation});
            }
        }
    }

    return shortestPaths;
}

vector<int> CampusCompass::getShortestPath(int source, int destination) {
    unordered_map<int, int> distances;
    unordered_map<int, int> parent;  // Track parent for path reconstruction
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

    distances[source] = 0;
    parent[source] = -1;  // source has no parent
    pq.push({0, source});

    while (!pq.empty()) {
        auto [currentDist, currentLocation] = pq.top();
        pq.pop();

        if (currentDist > distances[currentLocation]) {
            continue;
        }

        if (currentLocation == destination) {
            break;
        }

        if (!graph.count(currentLocation)) {
            continue;
        }

        for (const auto& [neighborLocation, edgeTime] : graph[currentLocation]) {
            if (closedEdges.count({currentLocation, neighborLocation})) {
                continue;
            }

            int newDist = currentDist + edgeTime;

            if (!distances.count(neighborLocation) || newDist < distances[neighborLocation]) {
                distances[neighborLocation] = newDist;
                parent[neighborLocation] = currentLocation;
                pq.push({newDist, neighborLocation});
            }
        }
    }
    vector<int> path;
    if (!parent.count(destination)) {
        return path;
    }
    int current = destination;
    while (current != -1) {
        path.push_back(current);
        current = parent[current];
    }
    reverse(path.begin(), path.end());
    return path;
}
int CampusCompass::find(int x) {
    if (!parent.count(x)) {
        parent[x] = x;
        rank[x] = 0;
    }
    if (parent[x] != x) {
        parent[x] = find(parent[x]);
    }
    return parent[x];
}

void CampusCompass::unionSets(int x, int y) {
    int rootX = find(x);
    int rootY = find(y);
    if (rootX == rootY) return;
    if (rank[rootX] < rank[rootY]) {
        parent[rootX] = rootY;
    } else if (rank[rootX] > rank[rootY]) {
        parent[rootY] = rootX;
    } else {
        parent[rootY] = rootX;
        rank[rootX]++;
    }
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
        if (line.empty() || line.find_first_not_of(" \n\r\t ") == string::npos) {
            continue;
        }
        stringstream ss(line);
        string classCode, startTime, endTime, locationStr;
        getline(ss, classCode, ',');
        getline(ss,locationStr, ',');
        getline(ss, startTime, ',');
        getline(ss, endTime);
        if (locationStr.empty()) {
            continue;
        }
        try {
            int locationID = stoi(locationStr);
            Class newClass;
            newClass.classCode = classCode;
            newClass.startTime = startTime;
            newClass.endTime = endTime;
            newClass.locationID = locationID;
            classes[classCode] = newClass;
        } catch (...) {
            continue;
        }
    }
    ifstream edgesFile(edges_filepath);
    if (!edgesFile.is_open()) {
        cerr<<"Error opening file "<<edges_filepath<<endl;
        return false;
    }
    while (getline(edgesFile,line)) {
        if (line.empty() || line.find_first_not_of(" \n\r\t ") == string::npos) {
            continue;
        }
        stringstream ss(line);
        string location1, location2, name1, name2, timeStr;
        getline(ss, location1, ',');
        getline(ss, location2, ',');
        getline(ss, name1, ',');
        getline(ss, name2, ',');
        getline(ss, timeStr);
        if (location1.empty() || location2.empty() || timeStr.empty()) {
            continue;
        }
     try {
         int loc1 = stoi(location1);
         int loc2 = stoi(location2);
         int time = stoi(timeStr);
         locations[loc1] = name1;
         locations[loc2] = name2;

         graph[loc1].push_back({loc2,time});
         graph[loc2].push_back({loc1,time});
     } catch (...) {
         continue;
     }
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
                    ++it;
                }
            }else {
                ++it;
            }
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
       if (graph.count(id1)) {
           for (auto neighbor: graph[id1]) {
               if (neighbor.first == id2) {
                   edgeStatus = true;
                   break;
               }
           }
       }
        if (!edgeStatus) {
            cout<<"DNE"<<endl;
            return true;
        }
        if (closedEdges.find({id1,id2}) != closedEdges.end() || closedEdges.find({id2,id1}) != closedEdges.end()) {
            cout<<"closed"<<endl;
            return true;
        }
        cout<<"open"<<endl;
        return true;
    }
    else if (Command == "isConnected") {
        int start, end;
        ss >> start >> end;
        queue<int> qu;
        qu.push(start);
        unordered_set<int> visited;
        visited.insert(start);
        bool isFound = false;
        while (!qu.empty()) {
            int temp = qu.front();
            qu.pop();
            if (temp == end) {
                isFound = true;
                break;
            }
            for (auto edge: graph[temp]) {
                int neighbor = edge.first;
                if (closedEdges.count({temp,neighbor}) || closedEdges.count({neighbor,temp})) {
                    continue;
                }
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    qu.push(neighbor);
                }
            }
        }
        if (isFound) {
            cout<<"successful"<<endl;
        }else {
            cout<<"unsuccessful"<<endl;
        }
        return true;
    }
    else if (Command =="printShortestEdges") {
        int studentID;
        ss >> studentID;
        if (to_string(studentID).length() != 8 || students.find(studentID) == students.end()) {
            cout<<"unsuccessful"<<endl;
            return false;
        }
        Student student = students[studentID];
        int sourceLocation = student.residenceID;

        unordered_map<int,int>distResidence = shortestPathsFrom(sourceLocation);
        vector<string> classCodes(student.classes.begin(), student.classes.end());
        sort(classCodes.begin(), classCodes.end());

        cout<< "Name: "<<student.name<<endl;
        for (string& code: classCodes) {
            const Class cls = classes[code];
            int classLocation = cls.locationID;
            int ETA = -1;
            auto it = distResidence.find(classLocation);
            if (it != distResidence.end()) {
                ETA = it->second;
            }
            cout<<code<< " | Total Time: "<<ETA<<endl;
        }
        return true;
    }
    else if (Command == "printStudentZone") {
        int studentID;
        ss >> studentID;
        Student student = students[studentID];
        int sourceLocation = student.residenceID;
        unordered_set<int> allVerts;
        allVerts.insert(sourceLocation);
        for (const string& classCode: student.classes) {
            int classLocation = classes[classCode].locationID;
            vector<int> path = getShortestPath(sourceLocation, classLocation);
            for (int vertex : path) {
                allVerts.insert(vertex);
            }
        }
        vector<tuple<int,int,int>> edges;
        for (int v: allVerts) {
            if (graph.count(v)) {
                for (const auto& [neighbor, weight] : graph[v]) {
                    if (allVerts.count(neighbor) && v < neighbor) {
                        edges.push_back({weight,v,neighbor});
                    }
                }
            }
        }
        sort(edges.begin(), edges.end());
        parent.clear();
        rank.clear();
        int MSTCost = 0;
        int edgesAdded = 0;
        int numOfVertices = 0;
        for (const auto& [weight, u, v]: edges) {
            if (find(u) != find(v)) {
                unionSets(u,v);
                MSTCost += weight;
                edgesAdded++;
                if (edgesAdded == numOfVertices - 1) {
                    break;
                }
            }
        }
        cout<<"Student Zone Cost For" << student.name << ": "<< MSTCost<<endl;
        return true;
    }
cout<<"unsuccessful"<<endl;
return false;

}
