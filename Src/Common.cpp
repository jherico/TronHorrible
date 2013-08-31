#include "Common.hpp"

using namespace std;

string slurp(const string& in) {
    ifstream ins;
    ins.open(in.c_str());
    return slurp(ins);
}

string slurp(ifstream& in) {
    stringstream sstr;
    sstr << in.rdbuf();
    return sstr.str();
}
