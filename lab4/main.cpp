#include "Schedalgo.h"
#include <iostream>
#include <unistd.h>
#include<cstdlib>
using namespace std;

int main(int argc, char* argv[])
{
    bool is_T=0;
    int c;
    Scheduler* sched;
    char *arg=NULL;
    opterr = 0;
    while((c=getopt(argc,argv,"s:"))!=-1)
    {
        switch(c)
        {
            case 's':
                arg=optarg;
                if(arg[0]=='i')
                    sched=new Scheduler(is_T);      //base class is actually FIFO
                else if(arg[0]=='j')
                    sched=new SSTF(is_T);
                else if(arg[0]=='s')
                    sched=new SCAN(is_T);
                else if(arg[0]=='c')
                    sched=new CSCAN(is_T);
                else if(arg[0]=='f')
                    sched=new FSCAN(is_T);
                else
                { cout<<"Wrong Options !(wrong algorithms?)"<<endl;
                    return 1;}
                break;
            case '?':
                cout<<"Wrong Options ! (missing option arguments?)"<<endl;
                return 1;
            default:
                 abort();
        }
    }
    
    
	if(!sched->readInput(argv[optind]))
    {
        delete sched;
        return 1;
    }
    sched->simulation();
    if(is_T)
        sched->printInfo();
    sched->printSum();
    delete sched;
	return 0;
}