#ifndef PROCESS_H
#define PROCESS_H


class Process
{
public:
	int timeStamp;
	int pid;
	int arrivalTime;
	int totalCPUTime;
    int CPUBurst;
	int IOBurst;

	int finishTime;
	int IOTime;                                             //store total io time
	int waitingTime;                                        //store total waiting time
	int remTime;
	int currentCPUBurst;
	int currentIOBurst;
	int stateTrans;                                         //store the state_transition
    int durTime;

	Process(int p, int at, int tc, int cb, int io);
	Process(){}
	bool operator < (const Process& p) const;
};

#endif