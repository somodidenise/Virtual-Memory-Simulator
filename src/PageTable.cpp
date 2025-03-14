#include "PageTable.h"
#include <stdexcept>

PageTable::PageTable(int totalEntries, int pageSize) :totalEntries(totalEntries), pageSize(pageSize) {
	entries.resize(totalEntries, -1); //
}

bool PageTable::isPresent(int entry) {
	return (entry >> 5) & 1;
}

bool PageTable::isProtected(int entry) {
	return (entry >> 6) & 1; 
}

bool PageTable::isModified(int entry) {
	return (entry >> 7) & 1; 
}

bool PageTable::isReferenced(int entry) {
	return (entry >> 8) & 1; 
}

int PageTable::getPageFrameNumber(int entry) {
	return entry & 0b11111; 
}

int PageTable::setBit(int entry, int bitPos, bool value) {
	if (value) {
		return entry | (1 << bitPos);
	}
	else {
		return entry & ~(1 << bitPos); 
	}
}

int PageTable::setPageFrameNumber(int entry, int frameNumber) {
	entry &= ~0b11111;       
	entry |= (frameNumber & 0b11111); 
	return entry;
}

int PageTable::getPhysicalAddress(int pageNumber, int offset, bool write) {
	//daca e absent=> page fault, dar phisicalMemory trebuie sa tina cont de cadrele care nu sunt prezente = un vector pt fiecare frame care are 1 daca e prezent
	//daca protection e read-only si se incearca o scriere => eroare
	//if (pageNumber < 0 || pageNumber >= totalEntries) {
		//throw std::out_of_range("Numarul paginii nu este corect");
	//}
	int entry = entries[pageNumber];
	if (entry == -1)
	{
		return -1;
	}
	if (!isPresent(entry)) {
		return -3; //page fault
	}
	if (isProtected(entry) && write) {
		return -2;
	}
	entry = setBit(entry, 8, true); //Referenced
	if (write) {
		entry = setBit(entry, 7, true); //Modified
	}
	entries[pageNumber] = entry;
	//Adresa fizica
	int pageFrameNr = getPageFrameNumber(entry);
	int physicalAddress = pageFrameNr * pageSize + offset;
	return physicalAddress;

}

void PageTable::setMapping(int pageNumber, int frameNumber, bool protection) {
	//if (pageNumber < 0 || pageNumber >= totalEntries) {
		//throw std::out_of_range("Numarul paginii nu este corect");
	//}
	int entry = 0;
	entry = setBit(entry, 5, true); //Present
	entry = setBit(entry, 6, protection);
	entry = setPageFrameNumber(entry, frameNumber);
	entries[pageNumber] = entry;
}

std::vector<int> PageTable::getEntryValues(int pageNumber) {
	int entry = entries[pageNumber];
	std::vector<int> values(6);

	values[0] = pageNumber;
	values[1] = isReferenced(entry);
	values[2] = isModified(entry);
	values[3] = isProtected(entry);
	values[4] = isPresent(entry);
	values[5] = getPageFrameNumber(entry);

	return values;
}

void PageTable::update(int pageNumber, bool write, bool absent)
{
	int entry = entries[pageNumber];
	
	if (absent == 1)
	{
		setBit(entry, 5, 0);
	}
	else {
		setBit(entry, 8, 1);
		if (write)
		{
			setBit(entry, 7, 1);
		}
	}

}

//
