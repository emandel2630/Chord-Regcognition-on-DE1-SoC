#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <string>
#include "sevensegment.h"

using namespace std;
const unsigned int HEX3_HEX0_OFFSET = 0x20;
const unsigned int HEX5_HEX4_OFFSET = 0x30;


const unsigned int bit_values[16]={119,0x7F7C,127,57,0x397E,63,0x797C, 121,113,0x717E,61,0x777C, 1, 0x15770E , 0x153037,0x5E3015};
//Contains all notes and chord types displayed on the board




SevenSegment :: SevenSegment(){
    reg0_hexValue =0;
    reg1_hexValue =0;
    RegisterWrite(HEX3_HEX0_OFFSET,reg0_hexValue);
    RegisterWrite(HEX5_HEX4_OFFSET,reg1_hexValue);
    cout << "SevenSegment Created" << endl;

}
SevenSegment::~SevenSegment(){
    Hex_ClearAll();
    cout << "SevenSegment Destroyed" << endl;

}
int SevenSegment :: Read1Switch(int switchNum){
    int value = RegisterRead(SW_OFFSET); //sets value to switch values
    int bitmask=1;
    
    bitmask = bitmask << switchNum; //left shifts bitmask to the switch of interest
    value = value & bitmask; //checks if bitmask and value are both 1 or not
    value = value >> switchNum; //right shifts value back as many times as the number of the switch
    return value; //returns 1 or 0 depending on if switch is on or off
}

void SevenSegment :: testvalues(){//Just a test
    reg0_hexValue =bit_values[15] + (bit_values[6] << 8) + (bit_values[1] << 16) + (bit_values[9] << 24);
    reg1_hexValue =bit_values[8] + (bit_values[2] << 8);
    RegisterWrite(HEX3_HEX0_OFFSET,reg0_hexValue);
    RegisterWrite(HEX5_HEX4_OFFSET,reg1_hexValue);
}
void SevenSegment:: Hex_ClearAll(){//Clears display
    reg0_hexValue =0;
    reg1_hexValue =0;
    RegisterWrite(HEX3_HEX0_OFFSET,reg0_hexValue);
    RegisterWrite(HEX5_HEX4_OFFSET,reg1_hexValue);
}
void SevenSegment :: Hex_ClearSpecific(int index){//Clears specific display
    int bitmask = 0x7F;
    int bitflip1 = 0xFFFFFFFF;
    int bitflip2 = 0xFFFF;
    if(index<4){//Shifts FFFF to correct spot and then reverses it so 7 seg shows 0
        bitmask = bitmask << (index*8);
        bitmask = bitmask ^ bitflip1;
        reg0_hexValue= reg0_hexValue & bitmask;
    }else{
        bitmask = bitmask << ((index-4)*8);
        bitmask = bitmask ^ bitflip2;
        reg1_hexValue = reg1_hexValue & bitmask;
    }

    RegisterWrite(HEX3_HEX0_OFFSET,reg0_hexValue);
    RegisterWrite(HEX5_HEX4_OFFSET,reg1_hexValue);

}
void SevenSegment :: Hex_WriteSpecific(int index, int value){//Writes to specific 7 seg display
    Hex_ClearSpecific(index);
    unsigned int bitmask = 0x7F;
    if(index<4){
        if (value >=0){
            bitmask = bit_values[value];
            bitmask = bitmask << (index *8);
            reg0_hexValue = reg0_hexValue | bitmask;
        }else{
            bitmask = bit_values[value+16];
            bitmask = bitmask << (index *8);
            reg0_hexValue = reg0_hexValue | bitmask;
        }
    }else{
        if (value >=0){
            bitmask = bit_values[value];
            bitmask = bitmask << ((index-4) *8);
            reg1_hexValue = reg1_hexValue | bitmask;
        }else{
            bitmask = bit_values[value+16];
            bitmask = bitmask << ((index-4) *8);
            reg1_hexValue = reg1_hexValue | bitmask;
        }
    }
    RegisterWrite(HEX3_HEX0_OFFSET,reg0_hexValue);
    RegisterWrite(HEX5_HEX4_OFFSET,reg1_hexValue);
}
void SevenSegment :: Hex_WriteNumber(int number){
    Hex_ClearAll();
    int bitmask =0xF;
    int display[6];
    if (number>=0){
        display[0] = number & bitmask;
        display[1]= (number >> 4) & bitmask;
        display[2]= (number >> 8) & bitmask;
        display[3]= (number >> 12) & bitmask;
        display[4]= (number >> 16) & bitmask;
        display[5]= (number >> 20) & bitmask;
        cout << endl <<endl;
        for(int j=0; j <6; j++){
            bool continuecheck =true;
            for(int i =0; i <16; i++){
                if(display[j] == i && continuecheck == true){
                    display[j] = bit_values[i];
                    continuecheck =false;
                }
            }
        }
    }else{
        number = number + 16777216;
        display[0] = number & bitmask;
        display[1]= (number >> 4) & bitmask;
        display[2]= (number >> 8) & bitmask;
        display[3]= (number >> 12) & bitmask;
        display[4]= (number >> 16) & bitmask;
        display[5]= (number >> 20) & bitmask;
        cout << endl <<endl;
        for(int j=0; j <6; j++){
            bool continuecheck =true;
            for(int i =0; i <16; i++){
                if(display[j] == i && continuecheck == true){
                    display[j] = bit_values[i];
                    continuecheck =false;
                }
            }
        }
    }
    reg0_hexValue = reg0_hexValue | display[0];
    reg0_hexValue = reg0_hexValue | (display[1] << 8);
    reg0_hexValue = reg0_hexValue | (display[2] << 16);
    reg0_hexValue = reg0_hexValue | (display[3] << 24);
    reg1_hexValue = reg1_hexValue | display[4];
    reg1_hexValue = reg1_hexValue | (display[5] << 8);
    RegisterWrite(HEX3_HEX0_OFFSET,reg0_hexValue);
    RegisterWrite(HEX5_HEX4_OFFSET,reg1_hexValue);

}

void SevenSegment :: ChordNoteDisplay(string Note){ //Based on the string that enters the corresponding 7 seg appears
    int value =12;
    if(Note.compare("A") == 0){
       value =0;
    }else if(Note.compare("Bflat/Asharp")== 0){
        value=1;
    }else if(Note.compare("B")== 0){
        value=2;
    }else if(Note.compare("C")== 0){
        value=3;
    }else if(Note.compare("Dflat/Csharp")== 0){
        value=4;
    }else if(Note.compare("D")== 0){
        value=5;
    }else if(Note.compare("Eflat/Dsharp")== 0){
        value=6;
    }else if(Note.compare("E")== 0){
        value=7;
    }else if(Note.compare("F")== 0){
        value=8;
    }else if(Note.compare("Gflat/Fsharp")== 0){
        value=9;
    }else if(Note.compare("G")== 0){
        value=10;
    }else if(Note.compare("Aflat/Gsharp")== 0){
        value=11;
    }else{
        value =12;
    }


    Hex_WriteSpecific(4, value);

}

void SevenSegment :: ChordTypeDisplay(string Type){ //Depending on chord the corresponding 7 seg turns on 
    int value =12;
    if(Type.compare("major") == 0){
        value =13;
    }else if(Type.compare("minor")== 0){
        value=14;
    }else if (Type.compare("diminished")==0){
        value =15;
    }
    else{
        value =12;
    }


    Hex_WriteSpecific(0, value);

}


