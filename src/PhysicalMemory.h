#pragma once
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <stdexcept>
class PhysicalMemory
{
private:
	//std::vector<std::vector<unsigned char>> addresses;
	std::queue<int> availableFrames;
	int memorySize;
	int memoryFrames;
	int totalFrames;
	//std::string replacementPolicy;
	std::queue<int> fifoQueue;
	std::list<int> lruList;

public:
	int frameSize;
	std::string replacementPolicy;
	PhysicalMemory(int memorySize, int totalFrames, int frameSize, std::string replacementPolicy);
	std::vector<std::vector<unsigned char>> addresses;
	std::vector<std::vector<unsigned char>> secondaryAddresses; //memoria secundara (disc)
	int addPageFrame(int virtualPageNumber);
	int getNextAvailableFrame();
	void write(int frameNumber, int offset,unsigned char data);
	unsigned char read(int frameNumber, int offset);
	void freeFrame(int frameNumber);
	bool isFull() const;
	int removePageFrame();
	void updateLRU(int frameNumber);
	void writeToSecondaryMem(int frameNumber);
};

