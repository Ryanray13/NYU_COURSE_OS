#include "algobase.h"
#include <iostream>
using namespace std;

Algobase::Algobase(struct PTE * p,vector<struct Frame> * f,int framenum)
{
	this->frame=f;
	this->page_table=p;
	this->frame_table=f->begin();
	this->frame_size=framenum;
	this->randofs=-1;
}


int Algobase::readRandom(char* path)
{
	this->inFile.open(path,ios_base::binary);
	if(!inFile){
		cout<<"Error! Cannot open the random number file!"<<endl;
		inFile.close();
		return 0;
	}
    inFile>>this->randCount;
	int temp;
	inFile>>temp;
	while(!inFile.eof())
	{
		this->randNum.push_back(temp);
		inFile>>temp;
	}
	inFile.clear();
	inFile.close();
	return 1;
}

int Algobase::getRand(int num)
{	
	randofs=(randofs+1)%randCount;
	return randNum[randofs]%num;
}