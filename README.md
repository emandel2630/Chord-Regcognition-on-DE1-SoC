# Audio to Chord Recognition Program

## Overview
This program is designed to convert audio input into a chord name using Digital Fourier Transforms (DFT) and frequency analysis. It's a sophisticated application intended for use with the De1-SOC board, involving peripheral interactions with a microphone, speakers, a keypad, and a 7-segment display.

## Setup Requirements

- **Hardware**: DE1-SoC board.
- **Input Device**: 1/8 inch headphone cable into the microphone (red) port.
- **Output Device**: 3 speakers attached to JP1 headers D0, D1, and D2 respectively, with grounding.
- **Interface Devices**: Keypad connected to pins D9-D21.

## Pre-requisites

1. **Chord.txt File**: A text file named "Chord.txt" must be present in the same folder as the program. This file should contain the audio data to be processed.
2. **Libraries**: Ensure the following libraries are included before compiling:
   - `<stdio.h>`
   - `<unistd.h>`
   - `<stdlib.h>`
   - `<fcntl.h>`
   - `<sys/mman.h>`
   - `<iostream>`
   - `<fstream>`
   - `<string>`
   - `<cstdio>`
   - `<ctime>`
   - `<cstdlib>`
   - `<complex>`
   - `<math.h>`
   - `<bits/stdc++.h>`
   - "audiotochord.h"

## Functionality

- **Recording and Processing**: The program records audio through the microphone and processes it to identify the chord being played. This is done by reducing the calculations of the Discrete Fourier Transform (DFT) and matching frequency components to known chord structures.
- **Display and Interaction**: The identified chord's root and type are displayed on a 7-segment display. The keypad allows users to interact with the program, choosing to play the identified chord or record a new one.
- **Error Handling**: The program includes checks and messages to prevent incorrect inputs or actions that could disrupt the process.

## Running the Program

1. **Start the Program**: Compile and run the program on the DE1-SoC board.
2. **Record Audio**: Press the specified button to start and stop recording.
3. **Chord Identification**: After recording, the program will process the audio and identify the chord.
4. **Display**: The identified chord will be displayed on the 7-segment display.
5. **Interaction**: Use the keypad to interact with the program, playing chords or recording new audio.

## Main Loop

The primary loop of the program involves:

- Prompting for DFT reduction factor from the user.
- Waiting for a recording command.
- Recording and processing audio to identify chords.
- Displaying and interacting with the result through the keypad and 7-segment display.

## Functions Overview

- **filereader**: Reads the "Chord.txt" file and stores the data in an array.
- **Arraylength**: Calculates the length of an array.
- **scalarProductMat**: Calculates the scalar product of a matrix.
- **DotProduct**: Computes the dot product of two arrays.
- **SumElementsInArray**: Returns the sum of elements in an array.
- **maxofarray**: Determines the maximum value in an array.
- **indexatmax**: Finds the index of the maximum value in an array.
- **audiotochord**: The core function that converts audio data into a chord name.
- **SplitString**: Divides a string into substrings based on spaces.

## Conclusion and Next Steps

This program represents a sophisticated application of audio processing and user interaction using hardware components. Future enhancements could involve improving chord detection accuracy, expanding the range of identifiable chords, or integrating more complex user interactions.

### Important Notes

Ensure that all connections are secure and correct before running the program. Misconnections can lead to incorrect behavior or damage to the hardware. Always refer to the DE1-SoC manual and safety guidelines when setting up your hardware environment.
