#include "MMU.H"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
using namespace std;

int main(int argc, char* argv[])
{
    bool is_O=0,is_P=0,is_F=0,is_S=1,is_a=0,is_p=0,is_f=0;
	int c,i=0,type=1,frame=32;
    char *arg=NULL;
    opterr = 0;
    while((c=getopt(argc,argv,"a:o:f:"))!=-1)
    {
        switch(c)
        {
            case 'a':
                arg=optarg;
                if(arg[0]=='N')
                  type=0;
                else if(arg[0]=='l')
                  type=1;
                else if(arg[0]=='r')
                  type=2;
                else if(arg[0]=='f')
                  type=3;
                else if(arg[0]=='s')
                  type=4;
                else if(arg[0]=='c')
                  type=5;
                else if(arg[0]=='C')
                  type=6;
                else if(arg[0]=='a')
                  type=7;
                else if(arg[0]=='A')
                  type=8;
                else
                  { cout<<"Wrong Options !(wrong algorithms?)"<<endl;
                      return 1;}
                break;
            case 'o':
                arg=optarg;
                i=0;
                while(arg[i]!='\0')
                {
                    if(arg[i]=='O')
                      is_O=1;
                    else if(arg[i]=='P')
                      is_P=1;
                    else if(arg[i]=='F')
                      is_F=1;
                    else if(arg[i]=='S')
                      is_S=1;
                    else if(arg[i]=='a')
                      is_a=1;
                    else if(arg[i]=='p')
                      is_p=1;
                    else if(arg[i]=='f')
                      is_f=1;
                    else
                      { cout<<"Wrong Options !(wrong options for output?)"<<endl;
                        return 1;}
                    i++;
                }
                break;
            case 'f':
                arg=optarg;
                if(arg[0]>57 || arg[0]<48)
                  { cout<<"Wrong Options !(wrong frame number?)"<<endl;
                    return 1;}
                frame=atoi(arg);
                break;
            case '?':
                cout<<"Wrong Options ! (missing option arguments?)"<<endl;
                return 1;
            default:
                abort();
        }
    }
    
    
	MMU mm(is_O,is_P,is_F,is_S,is_a,is_p,is_f,frame,type);
	if(!mm.readRandom(argv[optind+1]))
		return 1;
	if(!mm.readInput(argv[optind]))
		return 1;
	mm.simulation();
	return 0;
}