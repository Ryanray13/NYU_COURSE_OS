#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <list>
#include <vector>
#include<fstream>
#include "IOjob.h"

class Scheduler                        //base class, also FIFO
{
protected:
    bool isTrace;                      //trace output
    int time;                          //store current time
    int track;                         //store current track
    bool isMoving;                     //indicate whether head is moving
    bool isUp;                         //indicate direction of head
    int total_time;
    int total_move;
    double avg_turnaround;
    double avg_waittime;
    int max_waittime;
    int jobOffset;                     //indicate how many jobs have been put into wait queue
    int jobSize;
    std::ifstream inFile;
    std::vector<IOjob> jobQueue;       //queue for all the jobs
    std::list<IOjob> waitQueue;        //queue for jobs waiting
    std::list<IOjob>::iterator it;
public:
    Scheduler(bool t);
    int readInput(char* path);
    virtual void putJobs(IOjob j);    //put jobs into qait queue
    virtual void getJobs();           //get a job from wait queue
    virtual bool checkEmpty();        //check whether wait queue is empty
    virtual void popQueue(std::list<IOjob>::iterator & itt);
    void printSum();                  //print results
    void printInfo();                 //print all jobs info
    void simulation();                //simulation
    void update();                    //update waittime when a job is selected
};

#endif