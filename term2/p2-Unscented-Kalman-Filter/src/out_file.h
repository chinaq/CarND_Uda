#ifndef OutFile_H
#define OutFile_H

#include <iostream>
#include <fstream>
#include "ukf.h"

using namespace std;

class OutFile {
public:
    OutFile();
    virtual ~OutFile();
    void Save(string sensor_type, UKF& ukf);
    void Open();
    void Close();
    // void Full();
private:
    ofstream out_file_;
};
#endif