#ifndef ALGOBASE_H
#define ALGOBASE_H

#include <vector>
#include<fstream>
#include "PTE.H"
using namespace std;


class Algobase
{
protected:
	ifstream inFile;
	int frame_size;
	int randofs;
	vector<struct Frame> * frame;
	vector<struct Frame>::iterator frame_table;
	vector<int> randNum;                         //random number list
	struct PTE * page_table;
	int randCount;
public:
	Algobase(struct PTE * p,vector<struct Frame> * f,int framenum);
    virtual ~Algobase(){}
	virtual int getFrame()=0;
    int readRandom(char* path);
	int getRand(int num);
	virtual void printTrace(){}               //print frames order in FIFO, Second Alogirithm
	virtual void printHand(){}                //print Hand index in Clock
	virtual void printExtra(){}               //print counter information for Aging.
	virtual void updateTrace(int frame){}     //update trace for LRU
};

#endif