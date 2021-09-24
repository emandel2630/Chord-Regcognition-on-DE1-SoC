#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <complex> //https://www.geeksforgeeks.org/complex-numbers-c-set-1/
#include <math.h>
#include <bits/stdc++.h>
#include "audiotochord.h"

/* How to set up the De1-SOC to use this program:
 * Plug an 1/8 inch headphone cable into the microphone (red) port
 * attach 3 speakers to jp1 headers D0,D1, and D2 respectively while grounding all of them as well
 * Attach the keypad to pins D9-D21*/

//YOU MUST HAVE A TXT FILE NAMED "Chord.txt" IN THE SAME FOLDER WITH THIS PROGRAM
//DONT TRY TO RUN IT WITHOUT IT IT WILL NOT WORK

using namespace std;
//Function Declaration
void filereader(double array[],int size);
int Arraylength(complex<double> array[]);
void scalarProductMat(complex<double> mat[], complex<double> k,int size,complex<double> returnedarray[]);
complex<double> DotProduct(complex<double> arr1[], complex<double> arr2[],int size);
complex<double> SumElementsInArray(complex<double> arr[],int size);
double maxofarray(double arr[], int n);
int indexatmax(double arr[], int n);
string audiotochord(double DFTreduction);
string SplitString(string str, int wordnumber);


int main(void) {
    Audio *audio = new Audio; //Object that controls all audio related actions
    SevenSegment *sevensegment = new SevenSegment; //Object that controls all 7 segment related actions
    jp1 *keypad = new jp1; //Object that controls all keypad related actions
    jp1 *speaker = new jp1; //Object that controls all speaker related actions
    int record = 0;
    bool keypadloop= true;
    int buttonloop= 1;
    int KEY_value=0;
    char Keypad_value= '~';
    double DFTreduction=0;
    bool DFTreductionloop = true;


    string Identifiedchord;
    cout << endl << endl;


    while(DFTreductionloop==true){ //Main program loop
        cout << "By what factor do you want to reduce the DFT calculation? (1 is no reduction) (10 is recommended)" << endl;
        cin >> DFTreduction; // User can reduce DFT calcuation loop iterations by changing this value
        if (DFTreduction <=0 || DFTreduction >100){
            cout << "You cannot chose 0, negative numbers, or numbers larger than 100" << endl;
        }else{
            DFTreductionloop =false;
        }
    }


    while (buttonloop == 1) {
        cout << "Press KEY0 to begin recording" << endl;
        cout << "Press KEY2 to end the program" << endl;

        while (KEY_value ==0){
            KEY_value = audio->RegisterRead(KEY_OFFSET); //Reads button value if no button is being pressed
        }

        if (KEY_value == 0x1){
            audio->InitializeRecording(); //Allows audio to take in input
            record =1;
            audio->Record(record); // Records audio
            Identifiedchord =audiotochord(DFTreduction); //Takes audio and converts it into chord name
            cout << Identifiedchord << endl;
            cout << "\n" << "\n";

            string ChordNote;
            string ChordType;
            ChordNote = SplitString(Identifiedchord,0);//Divides identified chord into note
            ChordType = SplitString(Identifiedchord,1);//Divides identified chord into type of chord
            sevensegment-> Hex_ClearAll();

            sevensegment->ChordNoteDisplay(ChordNote);//7 seg displays root of chord
            sevensegment->ChordTypeDisplay(ChordType);//7 seg displays type of chord
            keypad->InitializeKeypad();//JP1 pins responsible for keypad set to proper input and output
            keypadloop= true;

            cout << "Pressing Keys 1-7 will play that chord relative to the identified chord" << endl;
            cout << "Pressing * will allow you to record a new chord" << endl << endl;

            cout << "Enter Key:" << endl;
            while(keypadloop == true){
                while (Keypad_value == '~'){//While no key is pressed
                    Keypad_value = keypad->identifykeypadpressed();//Return keypad value
                }
                cout << Keypad_value << endl;

                if (Keypad_value == '*'){//Breaks keypad loop and returns to audio recording loop
                        keypadloop = false;
                        Keypad_value = '~';
                        cout << "Keypad loop over" << endl;
                }else{
                    if (ChordNote.compare("Unidentified")==0){ //If chord is unidentified
                        cout << "Cannot play chords relative to an unidentified chord" << endl; //error message
                    }else if(Keypad_value == '1' || Keypad_value == '2' || Keypad_value == '3'|| Keypad_value == '4'|| Keypad_value == '5'|| Keypad_value == '6'|| Keypad_value == '7'){
                        sevensegment->Hex_ClearAll();
                        speaker->playchord(ChordNote,ChordType,Keypad_value); //If chord is identified the speaker plays the number chord relative to the root based on button press
                    }else{
                        cout << "That button has no function" << endl;
                    }
                }

                sevensegment->Hex_ClearAll();
                sevensegment->ChordNoteDisplay(ChordNote); //Returns to displaying root chord
                sevensegment->ChordTypeDisplay(ChordType);



                while (Keypad_value != '~'){
                    Keypad_value = keypad->identifykeypadpressed();
                }


            }
            record =0;
        }

        if(KEY_value == 0x4){
            buttonloop = 1;
        }


        while(KEY_value !=0){
            if(KEY_value == 0x4){
                buttonloop = 0;//Ends program
                audio->RegisterWrite(LEDR_OFFSET,0x0);//Clears led
            }
            KEY_value = audio->RegisterRead(KEY_OFFSET); //waits until button value is 0 and then then restarts button loop
        }


    }
    delete audio; //clears pointers
    delete sevensegment;
    delete keypad;
    delete speaker;
}

void filereader(double array[],int size){ //Fills and array with the contents of a file
    ifstream file("Chord.txt");
    int count = 0;
    double x;

    while (count < size && file >> x) {
        array[count++] = x;
    }


    // display the values stored in the array
    /*for (int i = 0; i < count; i++) {
        cout << array[i] << "\n";
    }*/

}

int Arraylength(complex<double> array[]){ //Determines the length of an array
    int x= *(&array +1)-array;
    return x;
}

void scalarProductMat(complex<double> mat[], complex<double> k,int size,complex<double> returnedarray[]){
    // scalar element is multiplied by the matrix
    for (int i = 0; i < size; i++)
        returnedarray[i] = mat[i] *k; //Multiplies each array element by a constant
}

complex<double> DotProduct(complex<double> arr1[], complex<double> arr2[],int size){//Dot product of array
    complex<double> DotProduct(0,0);
    for(int i; i< size; i++){
        DotProduct= DotProduct + (arr1[i] *arr2[i]); //Multiplies each same index element and adds the to running sum
    }
    return DotProduct;
}

complex<double> SumElementsInArray(complex<double> arr[],int size){ //Adds up all elements in an array
    complex<double> SumElementsInArray(0,0);
    for(int i=0; i< size; i++){
        SumElementsInArray+= arr[i]; //Running sum of all elements in an array
    }
    return SumElementsInArray;
}

double maxofarray(double arr[], int n){//Determines maximum of an array
    double max = arr[0];
    for (int i = 0; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }
    return max;
}

int indexatmax(double arr[], int n){//Determiens what the index of a max array value is
    double max = arr[0];
    int indexatmax=0;
    for (int i = 0; i < n; i++) {
        if (arr[i] > max) {
            max = arr[i];
            indexatmax =i;
        }
    }
    return indexatmax;
}
/*
 * The function below is the key component of the entire program.
 * It takes the DFT reduction value in and reads the text file containing audio data to output a string
 * This string contains the name of the chord in the audio input
 */


string audiotochord(double DFTreduction){
    const int RawDatasize = 10000;//Number of rows in the data file
    const int DFTsegment= 50;//Number of data points analyized at a time to find local maxes of DFT graph
    const int NumberofNotesCompared = 15;//Number of notes used to determine chord
    double pi =3.14159265358979;
    double e =2.7182818284;
    double elapsedtime; //Clock related variables
    double RawAudioData[RawDatasize];//Array that contains audio data from text file
    complex<double> Imaginarynumber (0,1);//This is the value i
    filereader(RawAudioData,RawDatasize);//Reads txt file and fills RawAudioData with each row


    double fs = 8000;//Sampling frequency of De1soc
    double L = 10000; //RawDatasize
    double dft_loop_reduction = DFTreduction; //set to 1 for no reduction
    double f[RawDatasize/2+1];

    for(int i=0; i<=RawDatasize/2; i++){
        f[i] = fs* (i/L);//Makes frequency array half of the size of raw audio file as negative frequencies are discared
    }

    complex<double> fourTime[RawDatasize];
    double inttodouble =0; //Used to prevent errors in comparing ints to doubles
    for(int i=0; i<=RawDatasize-1; i++){
        inttodouble=i;
        fourTime[i] = (inttodouble/L); //Fills fourTime with 10000 divisions of audio data size
        //cout << fourTime[i] << endl;
    }
    inttodouble=0;

    complex<double> fCoefs[RawDatasize];
    complex<double> csw[RawDatasize];
    complex<double> multipliedfourTime[RawDatasize];
    complex<double> signaltimescsw[RawDatasize];

    //Begin DFT clock
    clock_t start =clock();


    for (int i=1; i<=RawDatasize; i= i +dft_loop_reduction){//Calculates fourier coeficients
        inttodouble=i;
        scalarProductMat(fourTime,(-1.0*Imaginarynumber*2.0*pi*(inttodouble-1)),RawDatasize,multipliedfourTime);
        for(int j=0; j<RawDatasize;j++){
            csw[j]= multipliedfourTime[j];
        }
        for(int j=0; j<RawDatasize;j++){
            csw[j] = pow(e,csw[j]);//Forms complex sine wave
        }
        for(int j=0; j<RawDatasize;j++){
            signaltimescsw[j] = csw[j]*RawAudioData[j];
        }
        fCoefs[i] = SumElementsInArray(signaltimescsw,RawDatasize); //Fourier coefficients are the sum of complex sinewaves
    }

    double DFT[RawDatasize];

    for(int i=0;i<=RawDatasize;i++){
        DFT[i]= pow(abs(fCoefs[i]/L),2); //Takes magnitude of frequency specturm so there are no negative values
    }

    //End DFT clock
    clock_t end = clock();
    elapsedtime = double(end - start)/CLOCKS_PER_SEC;
    cout << "Time to create frequency spectrum: "<< elapsedtime << " seconds" << endl << endl;


    double Psegment[DFTsegment];
    int indexAtMaxY =0;
    double xValueAtMaxYValue =0.0;
    int Highfrequencyvalues=DFTreduction*5; //Removes highest frequencies from analysis as they are higher than necessary
    double ArrayofMaximums [RawDatasize/DFTsegment-Highfrequencyvalues+1];
    int ArrayofMaxIndicies [RawDatasize/DFTsegment-Highfrequencyvalues+1];

    //Linked list
    int linkedlistcount = NumberofNotesCompared;
    int linkedlistsize = NumberofNotesCompared;
    double PreLinkedChordFrequencies[linkedlistsize];




    for (int i =0; i<=RawDatasize/DFTsegment-Highfrequencyvalues; i++){//Finds maximum value for every 50 data points
        indexAtMaxY=0;
        for(int j=0; j<DFTsegment; j++){ //Checks each 50 point segment and marks max and the index the max occurs at
            Psegment[j] = DFT[(i*DFTsegment)+j+1];
            indexAtMaxY= indexatmax(Psegment,DFTsegment);
            indexAtMaxY = indexAtMaxY + DFTsegment*i +1;
            xValueAtMaxYValue = DFT[indexAtMaxY];
            ArrayofMaximums[i] = xValueAtMaxYValue; //Array that holds all maximum values
            ArrayofMaxIndicies[i] = indexAtMaxY; // Array that holds the indexes of all maximum values
        }
    }
    int FrequencyIndexAtMax =0;
    

    for (int i =0; i<NumberofNotesCompared; i++){ //Takes the number of maximums equivalent to value of NumberofNotesCompared
        indexAtMaxY= indexatmax(ArrayofMaximums,RawDatasize/DFTsegment-Highfrequencyvalues+1);
        FrequencyIndexAtMax =ArrayofMaxIndicies[indexAtMaxY];
        if(FrequencyIndexAtMax <RawDatasize/2){
          if(f[FrequencyIndexAtMax] < 2000 && f[FrequencyIndexAtMax] >16 && ArrayofMaximums[indexAtMaxY] > 1000000000000.00){ //If frequency is in accpetable range add it to chord frequencies
              PreLinkedChordFrequencies[i] =f[FrequencyIndexAtMax];
          }else{
              PreLinkedChordFrequencies[i] =0.0;
          }
      }else{
        PreLinkedChordFrequencies[i] =0;
        }
        ArrayofMaximums[indexAtMaxY] =0;
    }

    

    int arraysortsize = sizeof(PreLinkedChordFrequencies)/sizeof(PreLinkedChordFrequencies[0]);
    sort(PreLinkedChordFrequencies, PreLinkedChordFrequencies +arraysortsize); //Sorts chord frequencies from least to greatest
    //This sort is done because lower frequencies are given priority as the lowest frequency is always the root of the chord

// Removes all zeros from chord frequency list and then shortens array to save on processing time and memory
//Linked list portion
    double *ChordFrequencies;
    ChordFrequencies = new double[linkedlistsize];

    for(int i=0; i<linkedlistsize;i++){
        ChordFrequencies[i] = PreLinkedChordFrequencies[i];
    }

    cout << "ChordFrequencies" << endl;
    for (int i=0; i <NumberofNotesCompared; i++){
        cout << ChordFrequencies[i] << " "; //Print chord frequencies
    }

    int zerocounter=0;

    for(int i=0; i <linkedlistsize; i++){
        if(ChordFrequencies[i] ==0){
            zerocounter++;
        }
    }
    cout << endl << endl;

    for(int i=0; i<linkedlistsize; i++){
        if(i+zerocounter <linkedlistsize){
            ChordFrequencies[i] = ChordFrequencies[i+zerocounter];
        }else{
            ChordFrequencies[i] =0;
        }
    }

    if (linkedlistsize -zerocounter >=3){
        linkedlistcount = linkedlistsize -zerocounter;
    }else{
        linkedlistcount =3;
    }

    cout << "There are " << linkedlistcount << " viable frequencies" << endl << endl;


    double *nv;
    nv = new double[linkedlistcount];
    for(int i=0; i<linkedlistcount; i++){
        nv[i] = ChordFrequencies[i]; //New array filled with non zero frequency entries
    }
    delete[] ChordFrequencies;
    ChordFrequencies = nv;

    const int PostLinkedlistNumberofNotesCompared = linkedlistcount;



    cout << "ChordFrequencies" << endl;
    for (int i=0; i <PostLinkedlistNumberofNotesCompared; i++){
        cout << ChordFrequencies[i] << " "; //Print chord frequencies
    }

    cout << "\n" << "\n";




    //create all values for all notes on piano
    double Afrequencies[7];
    double BflatAsharpfrequencies[7];
    double Bfrequencies[7];
    double Cfrequencies[7];
    double DflatCsharpfrequencies[7];
    double Dfrequencies[7];
    double EflatDsharpfrequencies[7];
    double Efrequencies[7];
    double Ffrequencies[7];
    double GflatFsharpfrequencies[7];
    double Gfrequencies[7];
    double AflatGsharpfrequencies[7];

    double note=0.0;
//Each row below is each note halfstep calculated for 7 octaves
    for (int i=0; i<=6; i++){
        inttodouble = i;
        note = 440*pow(2,(((1+(12*inttodouble))-49)/12));//Formula to determine value of A
        Afrequencies[i]= note;
        note = 440*pow(2,(((2+(12*inttodouble))-49)/12));
        BflatAsharpfrequencies[i]= note;
        note = 440*pow(2,(((3+(12*inttodouble))-49)/12));
        Bfrequencies[i]= note;
        note = 440*pow(2,(((4+(12*inttodouble))-49)/12));
        Cfrequencies[i]= note;
        note = 440*pow(2,(((5+(12*inttodouble))-49)/12));
        DflatCsharpfrequencies[i]= note;
        note = 440*pow(2,(((6+(12*inttodouble))-49)/12));
        Dfrequencies[i]= note;
        note = 440*pow(2,(((7+(12*inttodouble))-49)/12));
        EflatDsharpfrequencies[i]= note;
        note = 440*pow(2,(((8+(12*inttodouble))-49)/12));
        Efrequencies[i]= note;
        note = 440*pow(2,(((9+(12*inttodouble))-49)/12));
        Ffrequencies[i]= note;
        note = 440*pow(2,(((10+(12*inttodouble))-49)/12));
        GflatFsharpfrequencies[i]= note;
        note = 440*pow(2,(((11+(12*inttodouble))-49)/12));
        Gfrequencies[i]= note;
        note = 440*pow(2,(((12+(12*inttodouble))-49)/12));
        AflatGsharpfrequencies[i]= note;
    }
    inttodouble=0;
    double NoteError=0;
    string ChordNotes[PostLinkedlistNumberofNotesCompared];

    for(int i =0; i<PostLinkedlistNumberofNotesCompared;i++){//Determines acceptable note error for each note
        //Acceptable note error increases with frequnecy as higher notes are farther apart frequency wise
        NoteError =0;
        if (ChordFrequencies[i] > 0 && ChordFrequencies[i]< 63) { //Acceptable error of pitch at each octave in Hz
            NoteError = 2;
        }else if (ChordFrequencies[i] > 63 && ChordFrequencies[i]< 127) {
            NoteError = 4;
        }else if (ChordFrequencies[i] > 127 && ChordFrequencies[i]< 255) {
            NoteError = 8;
        }else if (ChordFrequencies[i] > 255 && ChordFrequencies[i]< 511) {
            NoteError = 16;
        }else if (ChordFrequencies[i] > 511 && ChordFrequencies[i]< 1023) {
            NoteError = 32;
        }else if (ChordFrequencies[i] > 1023 && ChordFrequencies[i]< 2047) {
            NoteError = 64;
        }else if (ChordFrequencies[i] > 2047 && ChordFrequencies[i]< 10000) {
            NoteError = 128;
        }

        for (int j=0; j<=6; j++){ //Determines what note each frequency is using note error
            if ((Afrequencies[j]-NoteError <ChordFrequencies[i] && Afrequencies[j]+NoteError >ChordFrequencies[i]) && (abs(Afrequencies[j]-ChordFrequencies[i])< abs(BflatAsharpfrequencies[j]-ChordFrequencies[i]) && abs(Afrequencies[j]-ChordFrequencies[i])< abs(AflatGsharpfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="A";
            }
            if ((BflatAsharpfrequencies[j]-NoteError <ChordFrequencies[i] && BflatAsharpfrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(BflatAsharpfrequencies[j]-ChordFrequencies[i])< abs(Bfrequencies[j]-ChordFrequencies[i]) && abs(BflatAsharpfrequencies[j]-ChordFrequencies[i])< abs(Afrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="Bflat/Asharp";
            }
            if ((Bfrequencies[j]-NoteError <ChordFrequencies[i] && Bfrequencies[j]+NoteError >ChordFrequencies[i]) && (abs(Bfrequencies[j]-ChordFrequencies[i])< abs(Cfrequencies[j]-ChordFrequencies[i]) && abs(Bfrequencies[j]-ChordFrequencies[i])< abs(BflatAsharpfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="B";
            }
            if ((Cfrequencies[j]-NoteError <ChordFrequencies[i] && Cfrequencies[j]+NoteError >ChordFrequencies[i]) && (abs(Cfrequencies[j]-ChordFrequencies[i])< abs(DflatCsharpfrequencies[j]-ChordFrequencies[i]) && abs(Cfrequencies[j]-ChordFrequencies[i])< abs(Bfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="C";
            }
            if ((DflatCsharpfrequencies[j]-NoteError <ChordFrequencies[i] && DflatCsharpfrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(DflatCsharpfrequencies[j]-ChordFrequencies[i])< abs(Dfrequencies[j]-ChordFrequencies[i]) && abs(DflatCsharpfrequencies[j]-ChordFrequencies[i])< abs(Cfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="Dflat/Csharp";
            }
            if ((Dfrequencies[j]-NoteError <ChordFrequencies[i] && Dfrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(Dfrequencies[j]-ChordFrequencies[i])< abs(EflatDsharpfrequencies[j]-ChordFrequencies[i]) && abs(Dfrequencies[j]-ChordFrequencies[i])< abs(DflatCsharpfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="D";
            }
            if ((EflatDsharpfrequencies[j]-NoteError <ChordFrequencies[i] && EflatDsharpfrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(EflatDsharpfrequencies[j]-ChordFrequencies[i])< abs(Efrequencies[j]-ChordFrequencies[i]) && abs(EflatDsharpfrequencies[j]-ChordFrequencies[i])< abs(Dfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="Eflat/Dsharp";
            }
            if ((Efrequencies[j]-NoteError <ChordFrequencies[i] && Efrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(Efrequencies[j]-ChordFrequencies[i])< abs(Ffrequencies[j]-ChordFrequencies[i]) && abs(Efrequencies[j]-ChordFrequencies[i])< abs(EflatDsharpfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="E";
            }
            if ((Ffrequencies[j]-NoteError <ChordFrequencies[i] && Ffrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(Ffrequencies[j]-ChordFrequencies[i])< abs(GflatFsharpfrequencies[j]-ChordFrequencies[i]) && abs(Ffrequencies[j]-ChordFrequencies[i])< abs(Efrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="F";
            }
            if ((GflatFsharpfrequencies[j]-NoteError <ChordFrequencies[i] && GflatFsharpfrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(GflatFsharpfrequencies[j]-ChordFrequencies[i])< abs(Gfrequencies[j]-ChordFrequencies[i]) && abs(GflatFsharpfrequencies[j]-ChordFrequencies[i])< abs(Ffrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="Gflat/Fsharp";
            }
            if ((Gfrequencies[j]-NoteError <ChordFrequencies[i] && Gfrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(Gfrequencies[j]-ChordFrequencies[i])< abs(AflatGsharpfrequencies[j]-ChordFrequencies[i]) && abs(Gfrequencies[j]-ChordFrequencies[i])< abs(GflatFsharpfrequencies[j]-ChordFrequencies[i]))){
                ChordNotes[i]="G";
            }
            if ((AflatGsharpfrequencies[j]-NoteError <ChordFrequencies[i] && AflatGsharpfrequencies[j]+NoteError >ChordFrequencies[i])&& (abs(AflatGsharpfrequencies[j]-ChordFrequencies[i])< abs(Afrequencies[j]-ChordFrequencies[i]) && abs(AflatGsharpfrequencies[j]-ChordFrequencies[i])< abs(Gfrequencies[j]-ChordFrequencies[i]))) {
                ChordNotes[i] = "Aflat/Gsharp";
            }
            if(ChordFrequencies[i] < 16){
                ChordNotes[i] = "NaN";
            }
        }
    }



/* This portion of the code below from here until the definition of chords is the improvement on the prior method to determine the chord
 * The number of times the note appears is now taken into account
 * The more the note appears the higher priority it has and is then placed higher up in the chord
 * The 3 top notes contain the chord
*/

    delete[] ChordFrequencies;
    int ChordNoteNumberSize = sizeof(ChordNotes) / sizeof(ChordNotes[0]);

    bool check[ChordNoteNumberSize];
    int counterarray[ChordNoteNumberSize];
    for(int i= 0; i <ChordNoteNumberSize; i++){
        counterarray[i] =0;
    }

    string UniqueChordNotes[ChordNoteNumberSize];
    for(int i= 0; i <ChordNoteNumberSize; i++){//Makes an empty array
        UniqueChordNotes[i] = " ? ";
    }

    for(int i=0;i<ChordNoteNumberSize;i++){
        check[i] = 0;
    }
    for(int i=0; i<ChordNoteNumberSize; i++){//Determines the number of times each note appears
        if(check[i]== 1){
            continue;
        }
        int count = 1;

        for(int j = i+1; j<ChordNoteNumberSize; j++){
            if (ChordNotes[i].compare(ChordNotes[j]) == 0){
                check[j] = 1;
                count++;
            }
        }
        counterarray[i] = count; //Count of each note is filled into this array
        UniqueChordNotes[i] = ChordNotes[i];

        cout<<ChordNotes[i]<<" appears: " << count << " time(s)"<< endl;
    }
    cout << endl << endl;

    int comparisoncounterarray[ChordNoteNumberSize];
    for(int i =0; i <ChordNoteNumberSize; i++){
        comparisoncounterarray[i] = counterarray[i];
    }
    sort(counterarray, counterarray + ChordNoteNumberSize, greater<int>()); //Count array is sorted from least to greates

    string SortedUniqueChordNotes[ChordNoteNumberSize];

    for(int i=0; i<ChordNoteNumberSize; i++){ //Using the sorted count array the note that lines up with the number of appearence is brought to the top
        for (int j=0; j<ChordNoteNumberSize; j++){
            if(counterarray[i] == comparisoncounterarray[j] && UniqueChordNotes[j].compare(" ? ") != 0 && UniqueChordNotes[j].compare("NaN") != 0 && counterarray[i] !=0){
                comparisoncounterarray[j] =0;
                SortedUniqueChordNotes[i] = UniqueChordNotes[j];
                break;
            }
        }

    }
    



    for (int i=0; i <3; i++){
        ChordNotes[i] = SortedUniqueChordNotes[i]; //Chord notes are now sorted by rate of appearence
    }

    cout << "\n" << "\n";
//Definition of all chords
    string ListofChords[24] = {"A major", "A minor", "Bflat/Asharp major", "Bflat/Asharp minor", "B major", "B minor","C major", "C minor","Dflat/Csharp major", "Dflat/Csharp minor","D major", "D minor","Eflat/Dsharp major", "Eflat/Dsharp minor","E major", "E minor","F major", "F minor","Gflat/Fsharp major", "Gflat/Fsharp minor","G major", "G minor","Aflat/Gsharp major", "Aflat/Gsharp minor"};
    string Amajorcomponents[3] = {"A","Dflat/Csharp","E"};
    string Aminorcomponents[3] = {"A", "C", "E"};
    string BflatAsharpmajorcomponents[3] = {"Bflat/Asharp", "D", "F"};
    string BflatAsharpminorcomponents[3] = {"Bflat/Asharp", "Dflat/Csharp", "F"};
    string Bmajorcomponents[3] = {"B", "Eflat/Dsharp", "Gflat/Fsharp"};
    string Bminorcomponents[3] = {"B", "D", "Gflat/Fsharp"};
    string Cmajorcomponents[3] = {"C", "E", "G"};
    string Cminorcomponents[3] = {"C", "Eflat/Dsharp", "G"};
    string DflatCsharpmajorcomponents[3] = {"Dflat/Csharp", "F", "Aflat/Gsharp"};
    string DflatCsharpminorcomponents[3] = {"Dflat/Csharp", "E", "Aflat/Gsharp"};
    string Dmajorcomponents[3] = {"D", "Gflat/Fsharp", "A"};
    string Dminorcomponents[3] = {"D", "F", "A"};
    string EflatDsharpmajorcomponents[3] = {"Eflat/Dsharp", "G", "Bflat/Asharp"};
    string EflatDsharpminorcomponents[3] = {"Eflat/Dsharp", "Gflat/Fsharp", "Bflat/Asharp"};
    string Emajorcomponents[3] = {"E", "Aflat/Gsharp", "B"};
    string Eminorcomponents[3] = {"E", "G", "B"};
    string Fmajorcomponents[3] = {"F", "A", "C"};
    string Fminorcomponents[3] = {"F", "Aflat/Gsharp", "C"};
    string GflatFsharpmajorcomponents[3] = {"Gflat/Fsharp", "Bflat/Asharp", "Dflat/Csharp"};
    string GflatFsharpminorcomponents[3] = {"Gflat/Fsharp", "A", "Dflat/Csharp"};
    string Gmajorcomponents[3] = {"G", "B", "D"};
    string Gminorcomponents[3] = {"G", "Bflat/Asharp", "D"};
    string AflatGsharpmajorcomponents[3] = {"Aflat/Gsharp", "C", "Eflat/Dsharp"};
    string AflatGsharpminorcomponents[3] = {"Aflat/Gsharp", "B", "Eflat/Dsharp"};

    string ChordComponents[24][3]; //2D array that holds all chord componets for all 24 maj/min chords
    for (int j=0; j<3; j++){
        ChordComponents[0][j]=Amajorcomponents[j];
        ChordComponents[1][j]=Aminorcomponents[j];
        ChordComponents[2][j]=BflatAsharpmajorcomponents[j];
        ChordComponents[3][j]=BflatAsharpminorcomponents[j];
        ChordComponents[4][j]=Bmajorcomponents[j];
        ChordComponents[5][j]=Bminorcomponents[j];
        ChordComponents[6][j]=Cmajorcomponents[j];
        ChordComponents[7][j]=Cminorcomponents[j];
        ChordComponents[8][j]=DflatCsharpmajorcomponents[j];
        ChordComponents[9][j]=DflatCsharpminorcomponents[j];
        ChordComponents[10][j]=Dmajorcomponents[j];
        ChordComponents[11][j]=Dminorcomponents[j];
        ChordComponents[12][j]=EflatDsharpmajorcomponents[j];
        ChordComponents[13][j]=EflatDsharpminorcomponents[j];
        ChordComponents[14][j]=Emajorcomponents[j];
        ChordComponents[15][j]=Eminorcomponents[j];
        ChordComponents[16][j]=Fmajorcomponents[j];
        ChordComponents[17][j]=Fminorcomponents[j];
        ChordComponents[18][j]=GflatFsharpmajorcomponents[j];
        ChordComponents[19][j]=GflatFsharpminorcomponents[j];
        ChordComponents[20][j]=Gmajorcomponents[j];
        ChordComponents[21][j]=Gminorcomponents[j];
        ChordComponents[22][j]=AflatGsharpmajorcomponents[j];
        ChordComponents[23][j]=AflatGsharpminorcomponents[j];
    }
    string comparisonchord[3];
    for(int i=0;i<3;i++){
        comparisonchord[i] = "Q";
    }

    for (int i=0; i<3; i++){
        comparisonchord[i] = ChordNotes[i];//First 3 notes from ChordNotes is used to compare to list
    }


//This part was removed as the sort by frequency removed the need to prevent duplicates from appearing in chords

    /*for(int j=0; j<5; j++){
            if (comparisonchord[1] == comparisonchord[2] || comparisonchord[1] == comparisonchord[0]){
                if (comparisonchord[1] !=ChordNotes[j] && ChordNotes[j] !=comparisonchord[0] && ChordNotes[j] !=comparisonchord[2]){
                    comparisonchord[2] = ChordNotes[j];
                }
            }
        if (comparisonchord[2] == comparisonchord[0] || comparisonchord[2] == comparisonchord[1]){
            if (comparisonchord[2] !=ChordNotes[j] && ChordNotes[j] !=comparisonchord[0] && ChordNotes[j] !=comparisonchord[1]){
                comparisonchord[2] = ChordNotes[j];
            }
        }
    }*/

    cout << "comparisonchord" << endl;
    for(int j=0; j<=2;j++){
        cout << comparisonchord[j] << " ";
    }
    cout << "\n" << "\n";

    int isthisthecorrectchord=0;
    string CorrectlyIdentifiedChord ="Unidentified Chord";
    for(int i=0; i<24; i++){
        isthisthecorrectchord= 0;
        for(int j=0; j<3; j++){ //Checks comparison chord with all possible chords
            if(comparisonchord[j]== ChordComponents[i][0] || comparisonchord[j]== ChordComponents[i][1] || comparisonchord[j]== ChordComponents[i][2]){
                isthisthecorrectchord =isthisthecorrectchord+1;
            }else{
                isthisthecorrectchord= 0;
            }
        }
        if(isthisthecorrectchord ==3){//If all three notes align with a chord then the chord is identified
            CorrectlyIdentifiedChord=ListofChords[i];
        }
    }

    cout << "Identified Chord" << endl;
    return CorrectlyIdentifiedChord;
}

string SplitString(string str, int wordnumber){ //Function which divides a string at each space
    stringstream ss(str);
    string split;
    string output;
    int counter =0;
    while(ss >> split){
        if(counter == wordnumber){
            output =split;
        }
        counter++;
    }
    return output;
}