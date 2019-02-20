#include "out_file.h"
#include <iostream>
#include <fstream>

using namespace std;

OutFile::OutFile() {
}
OutFile::~OutFile() {}

void OutFile::Open() {
    string out_file_name_ = "my_out_file.csv";
    out_file_.open(out_file_name_.c_str(), ofstream::out);
    if (!out_file_.is_open()) {
        cerr << "Cannot open output file: " << out_file_name_ << endl;
        exit(EXIT_FAILURE);
    }
    out_file_ << "sensor_type,";
    out_file_ << "NIS";  
    out_file_ << "\n";

    // cout << "file open" << endl;
}

void OutFile::Save(string sensor_type, UKF& ukf) {
    out_file_ << sensor_type << ",";
    out_file_ << ukf.NIS;
    out_file_ << "\n";
}

void OutFile::Close() {
    // close files
    if (out_file_.is_open()) {
        out_file_.close();
    }
    // cout << "file close" << endl;
}

// void OutFile::Full() {
//     Open();
//     Save();
//     Close();
// }