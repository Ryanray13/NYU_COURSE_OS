#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "Simulator.h"
using namespace std;

//for print the result and indicate algorithm type
string Scheduler_Type[]={
	"FCFS",                             //type 0
	"RR",                               //type 1
	"LCFS",                             //...
	"SJF"
};

int main(int argc, char* argv[])
{
	int i,type=0,s=0,input=0,rfile=0,quantum=0;
	string q;
	bool verbose=0;
	for(i=1;i<argc;i++)
	{
		if(argv[i][0]=='-')
		{
			if(verbose && s!=0)
			{
				cout<<"Wrong option!"<<endl;
				return 1;
			}
			else if(argv[i][1]=='v' && !verbose)
				verbose=1;
			else if(argv[i][1]=='s'&& s==0)
			{
				if(argv[i][2]=='F')
					type=0;
				else if(argv[i][2]=='L')
					type=2;
				else if(argv[i][2]=='S')
					type=3;
				else if(argv[i][2]=='R')
				{
					type=1;
                    q=&argv[i][3];
					quantum=atoi(q.c_str());
				}
				else
				{
				    cout<<"Wrong option!<<endl";
				    return 1;
				}
			}
			else
			{
				cout<<"Wrong option!"<<endl;
				return 1;
			}
		}
		else
		{
			if(input==0)
				input=i;
			else if(rfile==0)
				rfile=i;
			else
			{
				cout<<"Wrong argument !"<<endl;
					return 1;
			}
		}
	}

	if(!input || !rfile)
	{
		cout<<"Wrong argument !"<<endl;
		return 1;
	}
    Simulator sim(type,verbose,quantum);
	if(!sim.readRandom(argv[rfile]))
		return 1;
	if(!sim.readInput(argv[input]))
		return 1;

	sim.dealEvent();
	sim.printResults();
	return 0;
}