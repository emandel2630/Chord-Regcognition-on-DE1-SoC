#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include "de1socfpga.h"
#include <string.h>


using namespace std;

class SevenSegment : public DE1SoCfpga{
private:
    unsigned int reg0_hexValue;
    unsigned int reg1_hexValue;

public:
    SevenSegment(); //Constructor
    ~SevenSegment(); //Destructor
    int Read1Switch(int switchNum);
    void testvalues();
    void Hex_ClearAll();
    void Hex_WriteSpecific(int index, int value);
    void Hex_ClearSpecific(int index);
    void Hex_WriteNumber(int number);
    void ChordNoteDisplay(string Note);
    void ChordTypeDisplay(string Type);
};

#endif

