#ifndef AUDIO_H
#define AUDIO_H

#include "de1socfpga.h"

class Audio : public DE1SoCfpga{
public:
    Audio();
    ~Audio();
    void InitializeRecording();
    void InitializePlayback();
    void Record(int record);
};

#endif
