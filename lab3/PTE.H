#ifndef PTE_H
#define PTE_H

struct PTE  {
	unsigned int present:1;
	unsigned int modified:1;
	unsigned int referenced:1;
	unsigned int pagedout:1;
	unsigned int index:6;

};

struct Frame{
	unsigned int page_index:6;
	bool isFree;
};

#endif