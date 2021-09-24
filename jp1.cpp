#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <string>
#include "jp1.h"


using namespace std;


jp1 :: jp1(){//Constructor
    cout << "JP1 connected" << endl;
    dataValue =0;
    directionValue=0;
    initialvalueLoadMPCore = RegisterRead(MPCORE_PRIV_TIMER_LOAD_OFFSET); //Sets up timer
    initialvalueControlMPCore = RegisterRead(MPCORE_PRIV_TIMER_CONTROL_OFFSET);
    initialvalueInterruptMPCore = RegisterRead(MPCORE_PRIV_TIMER_INTERRUPT_OFFSET);
    RegisterWrite(JP1_DIRECTION_REGISTER_OFFSET,directionValue);
}

jp1 ::~jp1(){//Destructor
    RegisterWrite(MPCORE_PRIV_TIMER_LOAD_OFFSET, initialvalueLoadMPCore);
    RegisterWrite(MPCORE_PRIV_TIMER_CONTROL_OFFSET, initialvalueControlMPCore);
    RegisterWrite(MPCORE_PRIV_TIMER_INTERRUPT_OFFSET, initialvalueInterruptMPCore);
    cout << "JP1 disconnected" << endl;
}

void jp1 :: AllOutput(){//Sets all jp1 pins to output
    directionValue = directionValue | 0xFFFFFFFF;
    RegisterWrite(JP1_DIRECTION_REGISTER_OFFSET,directionValue);
}

void jp1 :: InitializeKeypad(){//Sets all jp1 pins to correct configuration for keypad usage
    directionValue = 0;
    directionValue = directionValue | 0xFFFF95FF;
    RegisterWrite(JP1_DIRECTION_REGISTER_OFFSET,directionValue);

}

void jp1 :: jp1_WriteSpecific(int index, int state){//Write specific value to a specific pin
    unsigned int bitmask = 1;
    int bitflip = 0xFFFFFFF;
    if(state == 0){
        bitmask = bitmask << index;
        bitmask = bitmask ^ bitflip;
        dataValue = dataValue & bitmask;
    }else{
        bitmask = bitmask << index;
        dataValue = dataValue | bitmask;
    }
    RegisterWrite(JP1_DATA_REGISTER_OFFSET,dataValue);
    
}

char jp1 :: identifykeypadpressed(){//Program to identify which keypad button is pressed
    char keyout= '~';
    //By setting column values on and off and then checking the output of the rows the button pressed is determined
    jp1_WriteSpecific(15,0);//Output pins on keypad
    jp1_WriteSpecific(17,1);
    jp1_WriteSpecific(19,1);
    jp1_WriteSpecific(21,1);
    if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x2A6800){
         keyout = '1';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x2A6200){
         keyout = '4';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x2A4A00){
         keyout = '7';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x2A2A00){
         keyout = '*';
    }

    jp1_WriteSpecific(15,1);
    jp1_WriteSpecific(17,0);
    jp1_WriteSpecific(19,1);
    jp1_WriteSpecific(21,1);
    if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x28E800){
         keyout = '2';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x28E200){
         keyout = '5';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x28CA00){
         keyout = '8';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x28AA00){
         keyout = '0';
    }

    jp1_WriteSpecific(15,1);
    jp1_WriteSpecific(17,1);
    jp1_WriteSpecific(19,0);
    jp1_WriteSpecific(21,1);
    if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x22E800){
         keyout = '3';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x22E200){
         keyout = '6';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x22CA00){
         keyout = '9';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0x22AA00){
         keyout = '#';
    }

    jp1_WriteSpecific(15,1);
    jp1_WriteSpecific(17,1);
    jp1_WriteSpecific(19,1);
    jp1_WriteSpecific(21,0);
    if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0xAE800){
         keyout = 'A';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0xAE200){
         keyout = 'B';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0xACA00){
         keyout = 'C';
    }else if(RegisterRead(JP1_DATA_REGISTER_OFFSET) == 0xAAA00){
         keyout = 'D';
    }

    return keyout;
}

void jp1 :: playchord(string Note, string chordtype, char Keypad_value){ //Plays chord
    const unsigned int frequency_table[12]= {440,466.16,493.88,523.25,554.37,587.33,622.25,659.25,698.46,739.99,783.99,830.61}; //All notes from A4 to Aflat5

    int countlow = 0; // timeout = 1/(200 MHz) x 200x10^6 = 1 sec
    RegisterWrite(MPCORE_PRIV_TIMER_LOAD_OFFSET, countlow); //Offset occurs every count low
    RegisterWrite(MPCORE_PRIV_TIMER_CONTROL_OFFSET, 3);

    //AllOutput();
    
    int i = 0;
    int highlow1=0;
    int highlow2 =0;
    int highlow3=0;
    int ratio1 =0;
    int ratio2=0;
    int ratio3=0;
    int chordratiolcm;

    //Checks what chord has been input and sets value according to that
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
//Determines what chord needs to be played relative to either major or minor chord
    if(chordtype.compare("major")== 0){
        if(Keypad_value == '1'){
            ratio1=60; //Ratio of clock cycles needed to play major chord
            ratio2=48;
            ratio3=40;
            chordratiolcm = 60;
        }else if(Keypad_value == '2'){
            ratio1=60; //Ratio of clock cycles needed to play minor chord
            ratio2=50;
            ratio3=40;
            chordratiolcm = 60;
            value = value+2;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '3'){
            ratio1=60;
            ratio2=50;
            ratio3=40;
            chordratiolcm = 60;
            value = value+4;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '4'){
            ratio1=60;
            ratio2=48;
            ratio3=40;
            chordratiolcm = 60;
            value = value+5;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '5'){
            ratio1=60;
            ratio2=48;
            ratio3=40;
            chordratiolcm = 60;
            value = value+7;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '6'){
            ratio1=60;
            ratio2=50;
            ratio3=40;
            chordratiolcm = 60;
            value = value+9;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '7'){
            ratio1=60; //Ratio of clock cycles needed to play diminished chord
            ratio2=50;
            ratio3=42;
            chordratiolcm = 60;
            value = value+11;
            if(value>11){
                value=value-12;
            }

        }
    }else if(chordtype.compare("minor")== 0){
        if(Keypad_value == '1'){
            ratio1=60;
            ratio2=50;
            ratio3=40;
            chordratiolcm = 60;
        }else if(Keypad_value == '2'){
            ratio1=60;
            ratio2=50;
            ratio3=42;
            chordratiolcm = 60;
            value = value+2;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '3'){
            ratio1=60;
            ratio2=48;
            ratio3=40;
            chordratiolcm = 60;
            value = value+3;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '4'){
            ratio1=60;
            ratio2=50;
            ratio3=40;
            chordratiolcm = 60;
            value = value+5;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '5'){
            ratio1=60;
            ratio2=50;
            ratio3=40;
            chordratiolcm = 60;
            value = value+7;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '6'){
            ratio1=60;
            ratio2=48;
            ratio3=40;
            chordratiolcm = 60;
            value = value+8;
            if(value>11){
                value=value-12;
            }
        }else if(Keypad_value == '7'){
            ratio1=60;
            ratio2=48;
            ratio3=40;
            chordratiolcm = 60;
            value = value+10;
            if(value>11){
                value=value-12;
            }
        }
    }else{
        cout << "error" << endl;
    }
    Hex_ClearAll();
    Hex_WriteSpecific(4, value); //Writes played chord root to 7seg
    if(ratio3 ==42){ //depending on note ratios writes chord type to 7 seg
        Hex_WriteSpecific(0,15);
    }else if(ratio2 == 48){
        Hex_WriteSpecific(0,13);
    }else if(ratio2 ==50){
        Hex_WriteSpecific(0,14);
    }





    countlow = 200000000/frequency_table[value]/2/chordratiolcm; //Number of clock cycles before offset
    RegisterWrite(MPCORE_PRIV_TIMER_LOAD_OFFSET, countlow);
    RegisterWrite(MPCORE_PRIV_TIMER_CONTROL_OFFSET, 3);

    while (i < frequency_table[value]*chordratiolcm) {
        if (RegisterRead(MPCORE_PRIV_TIMER_INTERRUPT_OFFSET) != 0) {
            RegisterWrite(MPCORE_PRIV_TIMER_INTERRUPT_OFFSET, 1);
            i++;
            if (i % ratio1 == 0) {//Every ratio 1 clock cycles speaker 1 goes from high to low or low to high
                if(highlow1==0){
                    highlow1++;
                    jp1_WriteSpecific(0,0);
                }else{
                    jp1_WriteSpecific(0,1);
                    highlow1=0;
                }
            }
            if (i % ratio2 == 0) {//Every ratio 2 clock cycles speaker 2 goes from high to low or low to high
                if(highlow2==0){
                    highlow2++;
                    jp1_WriteSpecific(1,0);
                }else{
                    jp1_WriteSpecific(1,1);
                    highlow2=0;
                }
            }
            if (i % ratio3 == 0) {//Every ratio 3 clock cycles speaker 3 goes from high to low or low to high
                if(highlow3==0){
                    highlow3++;
                    jp1_WriteSpecific(2,0);
                }else{
                    jp1_WriteSpecific(2,1);
                    highlow3=0;
                }
            }

        }
    }


cout << "chord playing complete" << endl;
}
