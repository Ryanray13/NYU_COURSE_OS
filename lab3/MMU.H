#ifndef MMU_H
#define MMU_H

#include<fstream>
#include<vector>
#include "algoObject.h"
using namespace std;


class MMU
{
private:
	bool is_O;
	bool is_P;
	bool is_F;
	bool is_S;
	bool is_a;
	bool is_p;
	bool is_f;                                     //options                                
	unsigned long long totalcost;
	unsigned long instr_count;
    unsigned long unmaps;
	unsigned long maps;
	unsigned long ins;
	unsigned long outs;
	unsigned long zeros;
	int frame_size;
	int algo_type;                                      //algorithm type
    ifstream inFile;
	vector<int> instr;                             //instruction vector
	Algobase *faulthandler;
public:
	MMU(bool is_O,bool is_P,bool is_F,bool is_S,bool is_a,bool is_p,bool is_f, int frame,int type);
	~MMU();
	struct PTE page_table[64];
	vector<struct Frame> frame_table;
	void simulation();                             //main function of MMU.
	void printPageTable();
	void printFrameTable();
	void printSum();
	void printAging();
	void pageUpdate(int bit,int page);
	int readInput(char* path);
	int readRandom(char* path);
	int checkFreeFrame();                         //check whether there is any free frame.

};


#endif