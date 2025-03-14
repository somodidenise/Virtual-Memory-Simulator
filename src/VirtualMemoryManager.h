#pragma once
#include <string>
#include "VirtualAddressSpace.h"
#include "PhysicalMemory.h"
#include "PageTable.h"
#include "TLB.h"
class VirtualMemoryManager
{
private:
	int memorySize;
	int pageSize;
	int totalFrames;
	//std::string replacementPolicy;
	std::queue<int> fifoQueue;
	std::vector<int> lru;
public:
	VirtualAddressSpace virtualAddressSpace;
	PhysicalMemory physicalMemory;
	TLB tlb;
	PageTable pageTable;
	std::string replacementPolicy;
	VirtualMemoryManager(int memorySize, int pageSize, int totalFrames, std::string replacementPolicy);
	int translateAddress(int readAddress); //adresa fizica din adresa virtuala
	void removePage(); //scoate un page frame din RAM in functie de replacementPolicy
	void handlePageFault(int virtualPageNumber);
	void writeToAddress(int virtualAddress, char data);
	char readFromAddress(int virtualAddress);

};

