#ifndef SCHEDALGO_H
#define SCHEDALGO_H

#include "Scheduler.h"

//*****************************SSTF**************************************
class SSTF : public Scheduler
{
public:
    SSTF(bool t):Scheduler(t){}
    void getJobs();
};

//*****************************SCAN**************************************
class SCAN : public Scheduler
{
public:
    SCAN(bool t):Scheduler(t){}
    void getJobs();
	bool findUp();                     //find next job  up way
    bool findDown();                   //find next job  down way
};

//*****************************CSCAN*************************************
class CSCAN : public SCAN
{
public:
    CSCAN(bool t):SCAN(t){}
    void getJobs();
	void findMin();
};

//*****************************FSCAN*************************************
class FSCAN : public SCAN
{
private:
	std::list<IOjob> secondQueue;
	int indicator ;                            //indicator which queue we are using
public:
    FSCAN(bool t):SCAN(t){indicator=1;}
    void getJobs();
	void putJobs(IOjob j);
	bool checkEmpty();       
    void popQueue(std::list<IOjob>::iterator & itt);
	bool secfindUp();                          //find next job  up way in secondQueue
	bool secfindDown();                        //find next job  down way in secondQueue
};




#endif