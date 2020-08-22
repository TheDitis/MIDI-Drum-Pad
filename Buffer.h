//
// Created by Ryan McKay on 8/20/20.
//

#ifndef XIAOPIEZODRUM_BUFFER_H
#define XIAOPIEZODRUM_BUFFER_H
#include <vector>

using namespace std;

class Buffer {

public:
    Buffer();
    Buffer(int numValues);
    Buffer(int numValues, int smoothAmt);
//    void setNumValues(int numValues);
    double average();
    void add(int value);


private:
    int maxLength = 100;
    int smoothAmount = 10;
    vector <int> values;


};


#endif //XIAOPIEZODRUM_BUFFER_H
