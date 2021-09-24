#ifndef JP1_H
#define JP1_H

#include "de1socfpga.h"
#include "sevensegment.h"
#include <string.h>

using namespace std;

const unsigned int JP1_DATA_REGISTER_OFFSET = 0x60; // Points to JP1 DATA
const unsigned int JP1_DIRECTION_REGISTER_OFFSET = 0x64; // Points to JP1 DIRECTION
const unsigned int MPCORE_PRIV_TIMER_LOAD_OFFSET = 0xDEC600; // Points to LOAD
const unsigned int MPCORE_PRIV_TIMER_COUNTER_OFFSET = 0xDEC604; // Points to COUNTER
const unsigned int MPCORE_PRIV_TIMER_CONTROL_OFFSET = 0xDEC608; // Points to CONTROL
const unsigned int MPCORE_PRIV_TIMER_INTERRUPT_OFFSET = 0xDEC60C; // Points to INTERRUPT


class jp1 : public SevenSegment{
private:
    unsigned int dataValue;
    unsigned int directionValue; //0s are inputs 1s are outputs
    unsigned int initialvalueLoadMPCore;
    unsigned int initialvalueControlMPCore;
    unsigned int initialvalueInterruptMPCore;
public:
    jp1(); //Constructor
    ~jp1(); //Destructor
    void AllOutput();
    void jp1_WriteSpecific(int index, int state);
    void InitializeKeypad();
    char identifykeypadpressed();
    int readspecificindex(int index);
    void playchord(string chordnote, string chordtype, char Keypad_value);
};
#endif
