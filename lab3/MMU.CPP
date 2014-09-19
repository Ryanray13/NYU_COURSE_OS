#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <string>
#include <stdlib.h>
#include "MMU.H"
using namespace std;

MMU::MMU(bool is_O,bool is_P,bool is_F,bool is_S,bool is_a,bool is_p,bool is_f, int frame,int type)
{
    instr_count=0;
    unmaps=0;
	maps=0;
	ins=0;
    outs=0;
	zeros=0;
	algo_type=type;
	this->is_a=is_a;
	this->is_O=is_O;
	this->is_P=is_P;
	this->is_F=is_F;
	this->is_S=is_S;
	this->is_p=is_p;
	this->is_f=is_f;
	this->frame_size=frame;
	int i=0;
	struct Frame ftemp;
	for(i=0;i<64;i++)
	{
		page_table[i].index=0;
        page_table[i].modified=0;
		page_table[i].pagedout=0;
		page_table[i].present=0;
		page_table[i].referenced=0;
	}
	for(i=0;i<this->frame_size;i++)
	{
		ftemp.isFree=1;
		ftemp.page_index=0;
		this->frame_table.push_back(ftemp);
	}
	if(algo_type==0)
		this->faulthandler=new NRU(this->page_table,&this->frame_table,this->frame_size,is_a);
	else if(algo_type==2)
		this->faulthandler=new Random(this->page_table,&this->frame_table,this->frame_size);
	else if(algo_type==3)
		this->faulthandler=new FIFO(this->page_table,&this->frame_table,this->frame_size);
	else if(algo_type==4)
		this->faulthandler=new Second(this->page_table,&this->frame_table,this->frame_size);
	else if(algo_type==5)
		this->faulthandler=new PClock(this->page_table,&this->frame_table,this->frame_size);
	else if(algo_type==6)
		this->faulthandler=new VClock(this->page_table,&this->frame_table,this->frame_size);
	else if(algo_type==7)
		this->faulthandler=new PAging(this->page_table,&this->frame_table,this->frame_size,is_a);
	else if(algo_type==8)
	    this->faulthandler=new VAging(this->page_table,&this->frame_table,this->frame_size,is_a);
	else
        this->faulthandler=new LRU(this->page_table,&this->frame_table,this->frame_size);
}

int MMU::readInput(char* path)
{
	int i=0;
	this->inFile.open(path,ios_base::binary);
	if(!inFile){
		cout<<"Error! Cannot open the input file!"<<endl;
		inFile.close();	
		return 0;
	}

    char buffer[512];
    //string ss;
    //stringstream str;
	char* token;
	int temp;
    inFile.getline(buffer,512);
	while(!inFile.eof())
	{
        token=strtok(buffer," \n");
        
		if(token[0]!='#')
		{
			temp=atoi(token);
			this->instr.push_back(temp);
            token=strtok(NULL," \n");
            temp=atoi(token);
			this->instr.push_back(temp);
            
		}
		inFile.getline(buffer,512);

	}
	inFile.clear();
	inFile.close();
	return 1;
}
int MMU::readRandom(char* path)
{
	return this->faulthandler->readRandom(path);
}

void MMU::printPageTable()
{
	int i;
	for(i=0;i<64;i++)
	{
		if(page_table[i].present==1)
		{
	    	//cout<<i<<":";
            printf("%d:",i);
			if(page_table[i].referenced==1)
				//cout<<"R";
                printf("R");
			else
				//cout<<"-";
            printf("-");
			if(page_table[i].modified==1)
				//cout<<"M";
            printf("M");
			else
				//cout<<"-";
            printf("-");
			if(page_table[i].pagedout==1)
				//cout<<"S";
            printf("S");
			else
				//cout<<"-";
            printf("-");
		}
		else
		{
			if(page_table[i].pagedout==1)
			//	cout<<"#";
            printf("#");
			else
				//cout<<"*";
            printf("*");
		}
		//cout<<" ";
        printf(" ");
	}
	//cout<<endl;
    printf("\n");
}

void MMU::printFrameTable()
{
    int i;
	for(i=0;i<this->frame_size;i++)
	{
		if(frame_table[i].isFree==1)
			//cout<<"*";
        printf("*");
		else
			//cout<<frame_table[i].page_index;
        printf("%d",frame_table[i].page_index);
		//cout<<" ";
        printf(" ");
	}
}

void MMU::printSum()
{
	//cout<<"SUM "<<this->instr_count<<" U="<<this->unmaps<<" M="<<this->maps<<" I="<<this->ins<<" O="<<this->outs
	//	<<" Z="<<this->zeros<<" ===> ";
    printf("SUM %lu U=%lu M=%lu I=%lu O=%lu Z=%lu ===> ",instr_count,unmaps,maps,ins,outs,zeros);
	this->totalcost=(unmaps+maps)*400+(ins+outs)*3000+zeros*150+instr_count;
	//cout<<totalcost<<endl;
    printf("%llu\n",totalcost);
}

void MMU::pageUpdate(int bit,int page)
{
	if(bit==1)
		this->page_table[page].modified=1;
	this->page_table[page].referenced=1;
}

int MMU::checkFreeFrame()
{
	vector<struct Frame>::iterator it=this->frame_table.begin();
	while(it!=frame_table.end())
	{
		if(it->isFree)
			return it-frame_table.begin();
		++it;
	}
	return -1;

}

void MMU::simulation()
{
	int bit,page,frame,tpage;
	struct PTE * pp;
	struct PTE * ppt;

	vector<int>::iterator it=this->instr.begin();
    while(it!=instr.end())
	{

		bit=*it++;
		page=*it++;
		pp=&page_table[page];
		if(is_O)
			//cout<<"==> inst: "<<bit<<" "<<page<<endl;
            printf("==> inst: %d %d\n",bit,page);
		if(pp->present==0)
		{
			frame=checkFreeFrame();
			if(frame==-1)                                      //-1 means no free frame, need replacement.
			{
				frame=this->faulthandler->getFrame();          //no matter whether the algorithm is based on page or frame, for consistency it returns the frame index.
				tpage=frame_table[frame].page_index;
                ppt=&page_table[tpage];
				if(is_O)
					//cout<<instr_count<<": UNMAP"<<setw(4)<<tpage<<setw(4)<<frame<<endl;
                    printf("%lu: UNMAP%4d%4d\n",instr_count,tpage,frame);
				this->unmaps++;
				ppt->present=0;
                if(ppt->modified==1)
				{
					if(is_O)
						//cout<<instr_count<<": OUT"<<setw(6)<<tpage<<setw(4)<<frame<<endl;
                        printf("%lu: OUT%4d%4d\n",instr_count,tpage,frame);
					this->outs++;
					ppt->pagedout=1;
					ppt->modified=0;
				}
				ppt->referenced=0;
			}
			else                                                //there is free frame, use it.
				this->frame_table[frame].isFree=0;

            if(pp->pagedout==0)                    //if the page never pages out, zero the frame
			{	
				if(is_O)
					//cout<<instr_count<<": ZERO"<<setw(9)<<frame<<endl;
                printf("%lu: ZERO%9d\n",instr_count,frame);
				this->zeros++;
			}
			else
			{
				if(is_O)
					//cout<<instr_count<<": IN"<<setw(7)<<page<<setw(4)<<frame<<endl;
                printf("%lu: IN%7d%4d\n",instr_count,page,frame);
				this->ins++;
			}
	    	pp->present=1;	
			pp->index=frame;
			frame_table[frame].page_index=page;
			if(is_O)
				//cout<<instr_count<<": MAP"<<setw(6)<<page<<setw(4)<<frame<<endl;
                printf("%lu: MAP%6d%4d\n",instr_count,page,frame);
			this->maps++;	
		}

		pageUpdate(bit,page);
		if(algo_type==1)
			this->faulthandler->updateTrace(page_table[page].index);
		if(is_p)
			printPageTable();
		if(is_f)
		{
			printFrameTable();
			this->faulthandler->printTrace();
			this->faulthandler->printHand();
			this->faulthandler->printExtra();
			cout<<endl;
		}
		this->instr_count++;
	}
	if(is_P)
		this->printPageTable();
	if(is_F)
	{
		this->printFrameTable();
		//cout<<endl;
		printf("\n");
	}
	if(is_S)
		this->printSum();
}

MMU::~MMU()
{
	delete this->faulthandler;
}