#include "Scheduler.h"
#include<fstream>
#include<iostream>
#include<cstdlib>
#include<cstring>
using namespace std;


Scheduler::Scheduler(bool t )
{
    isTrace=t;
    max_waittime=0;
    total_move=0;
    total_time=0;
    avg_turnaround=0.0;
    avg_waittime=0.0;
    jobOffset=0;
    time=0;
    track=0;
    isMoving=0;
    isUp=1;
}

int Scheduler::readInput(char * path)
{
	int i=0;
    IOjob tjob;
    tjob.startTime=0;
    tjob.endTime=0;
	this->inFile.open(path,ios_base::binary);
	if(!inFile){
		cout<<"Error! Cannot open the input file!"<<endl;
		inFile.close();
		return 0;
	}
    
    char buffer[512];
	char* token;
	int temp;
    inFile.getline(buffer,512);
	while(!inFile.eof())
	{
        token=strtok(buffer," \n");

		if(token[0]!='#')
		{
			temp=atoi(token);
            tjob.timeStamp=temp;
            token=strtok(NULL," \n");
            temp=atoi(token);
            tjob.tracks=temp;
            tjob.jobid=i;
            i++;
			jobQueue.push_back(tjob);
            
		}
		inFile.getline(buffer,512);
        
	}
    jobSize=jobQueue.size();
	inFile.clear();
	inFile.close();
	return 1;
    
}


void Scheduler::printSum()
{
    avg_turnaround=1.0*avg_turnaround/jobSize;
    avg_waittime=1.0*avg_waittime/jobSize;
    
    printf("SUM: %d %d %.2lf %.2lf %d\n",total_time,total_move,avg_turnaround,avg_waittime,max_waittime);   
}

void Scheduler::printInfo()
{
    printf("IOREQS INFO\n");
    int i=0;
    for(;i<jobSize;i++)
        printf("%5d:%6d%6d%6d\n",jobQueue[i].jobid,jobQueue[i].timeStamp,jobQueue[i].startTime,jobQueue[i].endTime);
}

void Scheduler::putJobs(IOjob j)
{
    waitQueue.push_back(j);    
    if(isTrace)
        printf("%d:%6d add %d\n",time,j.jobid,j.tracks);  
}

void Scheduler::update()
{
    if(max_waittime<time-it->timeStamp)
        max_waittime=time-it->timeStamp;
    it->startTime=time;
    avg_waittime = avg_waittime + time - it->timeStamp;
    if(isTrace)
        printf("%d:%6d issue %d %d\n",time,it->jobid,it->tracks,track);
    if(it->tracks >track )
        isUp=1;
    else if(it->tracks < track)
        isUp=0;

    isMoving=1;
}

void Scheduler::getJobs()
{
    it=waitQueue.begin();

    update();
}

bool Scheduler::checkEmpty()
{
    if(waitQueue.empty())
        return true;
    else
        return false;
}

void Scheduler::popQueue(list<IOjob>::iterator& itt)
{
    waitQueue.erase(itt);
}

void Scheduler::simulation()
{
    int id;
    if(isTrace)
        printf("TRACE\n");
    while(1)
    {
        while(jobOffset!=jobSize && time==jobQueue[jobOffset].timeStamp)      //if we have a job put it into wait queue
        {
            putJobs(jobQueue[jobOffset]);
            jobOffset++;
        }
        
        if(!isMoving && !checkEmpty())                                         //if the head is not moving select a job
            getJobs();

        
        if(isMoving)                                                           //if the head is moving, check whether we have finish any jobs
        {
            while(track==it->tracks)
            {
                id=it->jobid;
                jobQueue[id].endTime=time;
                jobQueue[id].startTime=it->startTime;
                isMoving=0;
                if(isTrace)
                    printf("%d:%6d finish %d\n",time,it->jobid,time - it->timeStamp);
                avg_turnaround += time-it->timeStamp;
                popQueue(it);
                if(!checkEmpty())
                   getJobs();
                else
                    break;
            }
            
            if(isMoving)                                                        //keep moving
            {
                if(isUp)
                    track++;
                else
                    track--;
                total_move++;
            }
        }
        
        if(jobOffset==jobSize && checkEmpty())
            break;
        time++;
    }
    total_time=time;
}


