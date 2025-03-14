#pragma once
#include <vector>
class PageTable
{
private:
	std::vector<int> entries;
	//...|Refernced|Modified|Protection|Present/Absent|PageFrameNumber(5bits)
	int totalEntries;
	int pageSize;
	bool isPresent(int entry);
	bool isReferenced(int entry);
	bool isProtected(int entry);
	int setBit(int entry, int bitPos, bool value);
	int setPageFrameNumber(int entry, int frameNumber);
public:
	PageTable(int totalEntries, int pageSize);
	int getPhysicalAddress(int pageNumber, int offset, bool write); //VMM se ocupa de impartirea adresei virtuale propriu-zise
	void setMapping(int pageNumber, int frameNumber, bool protection);
	std::vector<int> getEntryValues(int pageNumber);
	void update(int pageNumber, bool write, bool absent);
	bool isModified(int entry);
	int getPageFrameNumber(int entry);

};

