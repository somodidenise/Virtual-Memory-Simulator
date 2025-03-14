#pragma once
#include <vector>
#include <deque>
class TLB
{
private:
	std::vector<int> entries;
	//..PageNumber(8 bits)|Refernced|Modified|Protection|Present/Absent|PageFrameNumber(5bits)
	int totalEntries;
	int pageSize;
	bool isPresent(int entry);
	bool isReferenced(int entry);
	bool isModified(int entry);
	bool isProtected(int entry);
	int getPageNumber(int entry);
	int getPageFrameNumber(int entry);
	int setBit(int entry, int bitPos, bool value);
	int setPageFrameNumber(int entry, int frameNumber);
	int setPageNumber(int entry, int pageNumber);
	std::deque<int> fifoQueue;
public:
	TLB(int pageSize);
	int getPhysicalAddress(int pageNumber, int offset, bool write); //VMM se ocupa de impartirea adresei virtuale propriu-zise
	void setMapping(int pageNumber, int frameNumber, bool protection);
	std::vector<int> getEntryValues(int pageNumber);
	int findIndex(int pageNumber);
	void update(int pageNumber, bool write, bool absent);
};

