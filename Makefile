# audiotochord

runaudiotochord: audiotochord.o de1socfpga.o jp1.o audio.o sevensegment.o
	g++ audiotochord.o audio.o de1socfpga.o jp1.o sevensegment.o -o runaudiotochord

de1socfpga.o: de1socfpga.cpp de1socfpga.h
	g++ -g -Wall -c de1socfpga.cpp

audio.o: audio.cpp audio.h
	g++ -g -Wall -c audio.cpp

sevensegment.o: sevensegment.cpp sevensegment.h
	g++ -g -Wall -c sevensegment.cpp

jp1.o: jp1.cpp jp1.h
	g++ -g -Wall -c jp1.cpp

audiotochord.o: audiotochord.cpp audiotochord.h
	g++ -g -Wall -c audiotochord.cpp

clean:
	rm audiotochord.o de1socfpga.o sevensegment.o jp1.o audio.o runaudiotochord