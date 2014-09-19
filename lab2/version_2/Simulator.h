#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <set>
#include <vector>
#include <fstream>
#include "SchedObject.h"
#include "Process.h"
using namespace std;

class Simulator
{
private:
	int timeStamp;
	bool isVerbose;                                 //store whether needs verbose print
	int quantum;
	int schedType;
	int blockCount;                                //count how many process in IO
	int totalIO;                                   //time at least one process is in IO
	int IObegin;                                   //time when one process begins IO
	multiset<Process> processList;                 //when process is finished, it will be stored in processList
	multiset<Process> eventQueue;                  //store event
	multiset<Process>::iterator it;
	vector<int> randNum;                           //random number list
    Scheduler *sched;                              //scheduler object
    ifstream inFile;

public:
	int randCount;                                 //store total randnum number
	int randofs;
	Simulator(int type, bool v=0, int quan=0);
	~Simulator();
	void printTrace(int io);
    void printResults();
    void getEvent();    
    void putEvent(Process& p);
	void putProcess(Process& p);                    //when process is finished, put it into processList
	void dealEvent();                               //main function of simulator, to deal every event in the event queue
	int readInput(char* path);
	int readRandom(char* path);
	void invokeSched();                             //function to invoke scheduler
	int getRand(int burst);

};

#endif