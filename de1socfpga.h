#ifndef DE1SOCFPGA_H
#define DE1SOCFPGA_H

// Important Message from Prof. Marpaung
// Read the PDF to find the real ADDRESS of LEDR, SW and KEY.
// End Important Message

// Physical base address of FPGA Devices
const unsigned int LW_BRIDGE_BASE 	= 0xFF200000;  // Base offset

// Length of memory-mapped IO window
const unsigned int LW_BRIDGE_SPAN 	= 0x00DEC700;  // Address map size

// Cyclone V FPGA device addresses
const unsigned int LEDR_OFFSET 		= 0x0;//real ADDRESS of RED LED - LW_BRIDGE_BASE ;
const unsigned int SW_OFFSET 		= 0x40;//real ADDRESS of SWITCH - LW_BRIDGE_BASE ;
const unsigned int KEY_OFFSET 		= 0x50;//real ADDRESS of PUSH BUTTON - LW_BRIDGE_BASE ;


class DE1SoCfpga
{
public:
    char *pBase;
    int fd;
    DE1SoCfpga(); //Constructor
    ~DE1SoCfpga(); //Destructor
    void RegisterWrite(unsigned int offset, int value);
    int RegisterRead(unsigned int offset);

};

#endif
