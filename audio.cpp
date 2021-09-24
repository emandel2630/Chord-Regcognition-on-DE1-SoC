#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <fstream>
#include "audio.h"


using namespace std;

const unsigned int AUDIO_OFFSET = 0x3040;
const unsigned int AUDIO_FIFOSPACE_OFFSET = 0x3044;
const unsigned int AUDIO_LEFT_OFFSET = 0x3048;
const unsigned int AUDIO_RIGHT_OFFSET = 0x304C;




Audio :: Audio(){ //Constructor
    cout << "Audio Created" << endl;

}

Audio ::~Audio(){ //Destructor
    cout << "Audio Destroyed" << endl;
}

void Audio :: InitializeRecording(){
    //Clear audio-in
    RegisterWrite(AUDIO_OFFSET,0x4);
    RegisterWrite(AUDIO_OFFSET,0x0);
    cout << "Recording..." << endl;
}

void Audio :: InitializePlayback(){
    //Clear audio-out
    RegisterWrite(AUDIO_OFFSET,0x8);
    RegisterWrite(AUDIO_OFFSET,0x0);
    cout << "Playing" << endl;
}

void Audio :: Record(int record){ //Sends Audio in data to text file
    const static int BUF_SIZE= 10000; // about 2 seconds of buffer (@ 8K samples/sec)
    int BUF_THRESHOLD= 96; // 75% of 128 word buffer
    int buffer_index= 0;
    double left_buffer[BUF_SIZE];
    double right_buffer[BUF_SIZE];
    int fifospace;
    int createrecording = record;
    while(createrecording==1) { //begin recording
        RegisterWrite(LEDR_OFFSET, 0x1); // turn on LEDR[0]
        fifospace = RegisterRead(AUDIO_FIFOSPACE_OFFSET); // read the audio port fifospace register
        if ((fifospace & 0x000000FF) > BUF_THRESHOLD) // check RARC
        {
// store data until the the audio-in FIFO is empty or the buffer
// is full
            while ((fifospace & 0x000000FF) && (buffer_index < BUF_SIZE)) {
                left_buffer[buffer_index] = RegisterRead(AUDIO_LEFT_OFFSET); //Fills left_buffer with audio data
                right_buffer[buffer_index] = RegisterRead(AUDIO_RIGHT_OFFSET);
                ++buffer_index;
                if (buffer_index == BUF_SIZE) { //when buffer is full
// done recording
                    createrecording = 0;
                    cout << "Done recording" << endl;
                    cout << "Calcuating..." << endl;

                    RegisterWrite(LEDR_OFFSET, 0x0);
                    ofstream audiofile;
                    audiofile.open("Chord.txt");
                    for (int i = 0; i < BUF_SIZE; i++) { //Audio data written to text file
                        audiofile << left_buffer[i] << "\n";
                    }
                    audiofile.close();
                }
                fifospace = RegisterRead(AUDIO_FIFOSPACE_OFFSET); // read the audio port fifospace register
            }
        }
    }
}

