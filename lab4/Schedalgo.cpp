#include "Schedalgo.h"
#include <list>
#include <cstdlib> 
#include <iostream>
using namespace std;

//*****************************SSTF**************************************
void SSTF::getJobs()
{
    list<IOjob>::iterator ittemp=waitQueue.begin();
	it=waitQueue.begin();
    int temp=abs(ittemp->tracks-track);
	++ittemp;
    for(;ittemp!=waitQueue.end();++ittemp)
    {
        if (temp >abs( ittemp->tracks - track))
		{
			temp=abs(ittemp->tracks-track);
			it=ittemp;			
		}
    }

    update();
}

//*****************************SCAN**************************************
bool SCAN::findUp()
{
	list<IOjob>::iterator ittemp=waitQueue.begin();
	it=waitQueue.end();
	int temp=2147480000;
	for(;ittemp!=waitQueue.end();++ittemp)
	{
		if(ittemp->tracks >= track)
			if (temp > ittemp->tracks - track)
			{
				temp=ittemp->tracks-track;
			    it=ittemp;			
			} 
	}
	if(it!=waitQueue.end())
		return true;
	else
		return false;
}

bool SCAN::findDown()
{
	list<IOjob>::iterator ittemp=waitQueue.begin();
	it=waitQueue.end();
	int temp=2147480000;
	for(;ittemp!=waitQueue.end();++ittemp)
	{
		if(ittemp->tracks <= track)
			if (temp > track - ittemp->tracks)
			{
				temp=track - ittemp->tracks;
			    it=ittemp;			
			} 
	}
	if(it!=waitQueue.end())
		return true;
	else
		return false;
}

void SCAN::getJobs()
{
	if(isUp)
	{
		if(!findUp())
			findDown();

	}
	else
	{
		if(!findDown())
			findUp();
	}
    update();
}

//*****************************CSCAN*************************************
void CSCAN::findMin()
{
	list<IOjob>::iterator ittemp=waitQueue.begin();
	it=ittemp;
	int temp=ittemp->tracks;
	++ittemp;
	for(;ittemp!=waitQueue.end();++ittemp)
	{
		if(temp > ittemp->tracks)
		{
			temp=ittemp->tracks;
			it=ittemp;
		}
	}
}

void CSCAN::getJobs()
{
	if(!findUp())
		findMin();
	//else
	//	isUp=1;
    update();
}

//*****************************FSCAN*************************************
bool FSCAN::checkEmpty()
{
	if(!waitQueue.empty() && secondQueue.empty())
	{
		if(indicator==1)
		{
			indicator=0;
		    isUp=1;
		}
		return false;
	}
	else if(!secondQueue.empty() && waitQueue.empty())
	{
		if(indicator==0)
		{
			indicator=1;
		    isUp=1;
		}
		return false;
	}
	else if(!waitQueue.empty() && !secondQueue.empty())
		return false;
	else
		return true;
}

void FSCAN::popQueue(list<IOjob>::iterator & itt)
{
	if(indicator==0)
		waitQueue.erase(itt);
	else
		secondQueue.erase(itt);
}

void FSCAN::putJobs(IOjob j)
{
	if(indicator==0)
		secondQueue.push_back(j);   
	else
        waitQueue.push_back(j);
	if(isTrace)
		printf("%d:%6d add %d\n",time,j.jobid,j.tracks);
}

bool FSCAN::secfindUp()
{
	list<IOjob>::iterator ittemp=secondQueue.begin();
	it=secondQueue.end();
	int temp=2147480000;
	for(;ittemp!=secondQueue.end();++ittemp)
	{
		if(ittemp->tracks >= track)
			if (temp > ittemp->tracks - track)
			{
				temp=ittemp->tracks-track;
			    it=ittemp;			
			} 
	}
	if(it!=secondQueue.end())
		return true;
	else
		return false;

}
	
bool FSCAN::secfindDown()
{
	list<IOjob>::iterator ittemp=secondQueue.begin();
	it=secondQueue.end();
	int temp=2147480000;
	for(;ittemp!=secondQueue.end();++ittemp)
	{
		if(ittemp->tracks <= track)
			if (temp > track - ittemp->tracks)
			{
				temp=track - ittemp->tracks;
			    it=ittemp;			
			} 
	}
	if(it!=secondQueue.end())
		return true;
	else
		return false;
}

void FSCAN::getJobs()
{
	if(indicator==0)
	{
		if(isUp)
		{
			if(!findUp())
				findDown();
		}
		else
		{
			if(!findDown())
				findUp();
		}
	}
	else
	{
		if(isUp)
		{
			if(!secfindUp())
				secfindDown();
		}
		else
		{
			if(!secfindDown())
				secfindUp();
		}
	}
    update();
}

