#include "algoObject.h"
#include<vector>
#include<iostream>
using namespace std;

//*****************************NRU***************************************
NRU::NRU(struct PTE * p,vector<struct Frame> * f,int framenum,bool a):Algobase(p,f,framenum)
{
	this->replaceCount=0;
	this->is_a=a;
	int i,j;
	for(i=0;i<4;i++)
		this->num_array[i]=0;
	for(j=0;j<4;j++)
		for(i=0;i<64;i++)
			this->page_array[j][i]=0;
}


int NRU::getLowestClass()
{
	int i=0;
	for(i=0;i<4;i++)
		if(this->num_array[i]!=0)
			return i;
	return i;
}

void NRU::resetPage()
{
	int i;
	for(i=0;i<64;i++)
		this->page_table[i].referenced=0;
}

void NRU::buildArray()
{
	int i,j=0,k=0,m=0,n=0;
	for(i=0;i<4;i++)
		this->num_array[i]=0;
	for(i=0;i<64;i++)
	{
		if(page_table[i].present==0)
			continue ;
		if(this->page_table[i].referenced==1)
		{
			if(page_table[i].modified==1)
			{
				num_array[3]++;
                page_array[3][j]=i;
				j++;
			}
			else
			{
				num_array[2]++;
                page_array[2][k]=i;
				k++;
			}
		}
		else
		{
			if(page_table[i].modified==1)
			{
				num_array[1]++;
                page_array[1][m]=i;
				m++;
			}
			else
			{
				num_array[0]++;
                page_array[0][n]=i;
				n++;
			}
		}
	}
}

int NRU::getFrame()
{
	int ptemp,i,r,j;
	buildArray();		
	replaceCount++;
	i=this->getLowestClass();
	r=this->getRand(num_array[i]);
	ptemp=page_array[i][r];
	if(this->replaceCount%10==0)
	{
		resetPage();
        if(is_a)
		{
			//cout<<" @@ reset NRU refbits while walking PTE"<<endl;
			printf(" @@ reset NRU refbits while walking PTE\n");
		}
	}
	if(is_a)
	{
	//	cout<<" @@ lowest_class="<<i<<": selidx="<<r<<" from";
		printf(" @@ lowest_class=%d: selidx=%d from",i,r);
		for(j=0;j<num_array[i];j++)
		//	cout<<" "<<page_array[i][j];
		    printf(" %d",page_array[i][j]);
	//	cout<<endl;
	    printf("\n");
	}
	return this->page_table[ptemp].index;             //even though the algorithm is based on virtual page, but for convenience we still 
	                                                  //return the frame inde for MMU to deal with.
}

//*****************************LRU***************************************
LRU::LRU(struct PTE * p,vector<struct Frame> * f,int framenum):Algobase(p,f,framenum)
{
	int i;
	for(i=0;i<this->frame_size;i++)
		this->frame_trace.push_back(i);
}

int LRU::getFrame()
{
	int ftemp=frame_trace.front();
	return ftemp;
}

void LRU::updateTrace(int frame)
{
	vector<int>::iterator it=this->frame_trace.begin();
	while(it!=frame_trace.end())
	{
		if(frame==*it)
			break;
		++it;
	}

	frame_trace.erase(it);
	frame_trace.push_back(frame);
}

//*****************************Random************************************
int Random::getFrame()
{	
	return this->getRand(this->frame_size);
}

//*****************************FIFO**************************************
FIFO::FIFO(struct PTE * p,vector<struct Frame> * f,int framenum):Algobase(p,f,framenum)
{
	int i;
	for(i=0;i<this->frame_size;i++)
		this->frame_trace.push_back(i);
}

int FIFO::getFrame()
{
	int ftemp=frame_trace.front();
	frame_trace.push_back(ftemp);
	frame_trace.erase(frame_trace.begin());
	return ftemp;
}

void FIFO::printTrace()
{
	int i;
	cout<<" || ";
	for(i=0;i<frame_size;i++)
		if(this->frame_table[frame_trace[i]].isFree==0)
		    cout<<frame_trace[i]<<" ";
}

//*****************************Second************************************
int Second::getFrame()
{
	int ftemp=frame_trace.front();	
	while(page_table[frame_table[ftemp].page_index].referenced==1)
	{
		page_table[frame_table[ftemp].page_index].referenced=0;
		frame_trace.push_back(ftemp);
	    frame_trace.erase(frame_trace.begin());
		ftemp=frame_trace.front();
	}
	frame_trace.push_back(ftemp);
	frame_trace.erase(frame_trace.begin());
	return ftemp;
}

//*****************************PClock************************************
PClock::PClock(struct PTE * p,vector<struct Frame> * f,int framenum):Algobase(p,f,framenum)
{
	this->hand=0;
}

int PClock::getFrame()
{
	int ptemp=frame_table[hand].page_index;
	while(page_table[ptemp].referenced==1)
	{
		page_table[ptemp].referenced=0;
		hand=(hand+1)%frame_size;                             //hand is based on frame table
		ptemp=frame_table[hand].page_index;
	}
	int ftemp=hand;
    hand=(hand+1)%frame_size;
	return ftemp;
}

void PClock::printHand()
{
	cout<<" || hand = "<<hand;
}

//*****************************VClock************************************
int VClock::getFrame()
{
	while(page_table[hand].referenced==1 || page_table[hand].present==0)
	{
		if(page_table[hand].referenced==1)
			page_table[hand].referenced=0;
		hand=(hand+1)%64;                                     //hand is based on page table
	}
	int ptemp=hand;
    hand=(hand+1)%64;
	return page_table[ptemp].index;
}

//*****************************PAging************************************  
PAging::PAging(struct PTE * p,vector<struct Frame> * f,int framenum,bool a):Algobase(p,f,framenum)
{
	this->is_a=a;
	int i;
	for(i=0;i<frame_size;i++)
		this->age_vector.push_back(0);
}

int PAging::getSmallest()
{
	unsigned long int temp=age_vector[0];
	int i,k=0;
	for(i=1;i<frame_size;i++)
	{
		if(temp>age_vector[i])
		{
			k=i;
			temp=age_vector[i];
		}
	}
	return k;
}

int PAging::getFrame()
{
	int i;
	for(i=0;i<frame_size;i++)
	{
		age_vector[i] >>= 1;
		if(page_table[frame_table[i].page_index].referenced==1)
		{
			age_vector[i] += 0x80000000 ;
            page_table[frame_table[i].page_index].referenced=0;
		}
	}
	int ftemp=this->getSmallest();
	
	if(is_a)
	{
		cout<<" @@ ";
		for(i=0;i<frame_size;i++)
			cout<<" "<<i<<":"<<std::hex<<age_vector[i]<<std::dec;
		cout<<endl;
		cout<<" @@ min_frame = "<<ftemp<<" age="<<std::hex<<age_vector[ftemp]<<std::dec<<endl;
	}
    age_vector[ftemp]=0;
	return ftemp;
}

void PAging::printExtra()
{
    int i;
	cout<<" || ";
	for(i=0;i<frame_size;i++)
		if(this->frame_table[i].isFree==0)
			cout<<i<<":"<<std::hex<<age_vector[i]<<std::dec<<" ";
}

//*****************************VAging************************************ 
VAging::VAging(struct PTE * p,vector<struct Frame> * f,int framenum,bool a):PAging(p,f,framenum,a)
{
	age_vector.clear();
	int i;
	for(i=0;i<64;i++)
		age_vector.push_back(0);
}

int VAging::getSmallest()
{
	int i=0,k=0;
	while(page_table[i].present==0){i++;}
    unsigned long int temp=age_vector[i];
	k=i;
	for(i=i+1;i<64;i++)
	{
		if(page_table[i].present==1)
		    if(temp>age_vector[i])
			{
			    k=i;
			    temp=age_vector[i];
			}
	}
	return k;
}

int VAging::getFrame()
{
	int i;
	for(i=0;i<64;i++)
	{
		if(page_table[i].present==1)
		{
		    age_vector[i] >>= 1;
		    if(page_table[i].referenced==1)
			{
			    age_vector[i] += 0x80000000 ;
                page_table[i].referenced=0;
			}
		}
	}
	int ptemp=this->getSmallest();
	
	if(is_a)
	{
		cout<<" @@ ";
		for(i=0;i<64;i++)
		{
			if(page_table[i].present==1)
				cout<<" "<<i<<":"<<std::hex<<age_vector[i]<<std::dec;
			else
				cout<<" *";
		}
		cout<<endl;
		cout<<" @@ min_pte = "<<ptemp<<" age="<<std::hex<<age_vector[ptemp]<<std::dec<<endl;
	}
    age_vector[ptemp]=0;
	return page_table[ptemp].index;
}

void VAging::printExtra()
{
    int i;
	cout<<" ||";
	for(i=0;i<64;i++)
	{
		if(this->page_table[i].present==1)
			cout<<" "<<i<<":"<<std::hex<<age_vector[i]<<std::dec;
		else
			cout<<" *";
	}
}