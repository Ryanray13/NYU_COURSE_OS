#ifndef ALGOOBJECT_H
#define ALGOOBJECT_H

#include "algobase.h"
using namespace std;



//*****************************NRU***************************************
class NRU : public Algobase
{
private:
	unsigned long replaceCount;
	int num_array[4];
    int page_array[4][64];
	bool is_a;
public:
	NRU(struct PTE * p,vector<struct Frame> * f,int framenum, bool a);
	int getFrame();
	void resetPage();                      //reset all the reference bit
	void buildArray();
	int getLowestClass();                  //get currently lowest class(non-empty class).
};

//*****************************LRU***************************************
class LRU : public Algobase
{
private:
	vector<int> frame_trace;
public:
	LRU(struct PTE * p,vector<struct Frame> * f,int framenum);
	int getFrame();
	void updateTrace(int frame);
};

//*****************************Random************************************
class Random : public Algobase
{
public:
	Random(struct PTE * p,vector<struct Frame> * f,int framenum):Algobase(p,f,framenum){}
	int getFrame();
};

//*****************************FIFO**************************************
class FIFO : public Algobase
{
protected:
	vector<int> frame_trace;                             //the order of frame in FIFO algorithm
public:
	FIFO(struct PTE * p,vector<struct Frame> * f,int framenum);
	int getFrame();
	void printTrace();
};

//*****************************Second************************************
class Second : public FIFO
{                          
public:
	Second(struct PTE * p,vector<struct Frame> * f,int framenum):FIFO(p,f,framenum){}
	int getFrame();
};

//*****************************PClock************************************      //Clock based on frames
class PClock : public Algobase
{    
protected:
	int hand;
public:
	PClock(struct PTE * p,vector<struct Frame> * f,int framenum);
	int getFrame();
	void printHand();
};

//*****************************VClock************************************      //Clock based on pages
class VClock : public PClock
{    
public:
	VClock(struct PTE * p,vector<struct Frame> * f,int framenum):PClock(p,f,framenum){}
	int getFrame();
};

//*****************************PAging************************************    //Aging base on frames
class PAging : public Algobase
{    
protected:
	bool is_a;
    vector<unsigned long int > age_vector;
public:
	PAging(struct PTE * p,vector<struct Frame> * f,int framenum, bool a);
	int getFrame();
	void printExtra();
    int getSmallest();
};

//*****************************VAging************************************    //Aging base on pages
class VAging : public PAging
{    
public:
	VAging(struct PTE * p,vector<struct Frame> * f,int framenum, bool a);
	int getFrame();
	void printExtra();
	int getSmallest();
};


#endif