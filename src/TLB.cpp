#include "TLB.h"
#include <stdexcept>
#include <deque>
#include <QDebug>

TLB::TLB(int pageSize) : pageSize(pageSize) {
	totalEntries = 6;
	entries.resize(totalEntries, -1);
}

bool TLB::isPresent(int entry) {
	return (entry >> 5) & 1;
}

bool TLB::isProtected(int entry) {
	return (entry >> 6) & 1;
}

bool TLB::isModified(int entry) {
	return (entry >> 7) & 1;
}

bool TLB::isReferenced(int entry) {
	return (entry >> 8) & 1;
}

int TLB::getPageNumber(int entry) {
	return (entry >> 9) & 0xFF;
}

int TLB::getPageFrameNumber(int entry) {
	return entry & 0b11111;
}

int TLB::setBit(int entry, int bitPos, bool value) {
	if (value) {
		return entry | (1 << bitPos);
	}
	else {
		return entry & ~(1 << bitPos);
	}
}

int TLB::setPageFrameNumber(int entry, int frameNumber) {
	entry &= ~0b11111;
	entry |= (frameNumber & 0b11111);
	return entry;
}

int TLB::setPageNumber(int entry, int pageNumber) {
	entry &= ~(0xFF << 9);           
	entry |= (pageNumber & 0xFF) << 9; 
	return entry;
}

int TLB::getPhysicalAddress(int pageNumber, int offset, bool write) {
	//daca e absent=> page fault, dar phisicalMemory trebuie sa tina cont de cadrele care nu sunt prezente = un vector pt fiecare frame care are 1 daca e prezent
	//daca protection e read-only si se incearca o scriere => eroare
	//if (pageNumber < 0 || pageNumber >= totalEntries) {
		//throw std::out_of_range("Numarul paginii nu este corect");
	//}
	qDebug() << "page number:" << pageNumber;
	int i = findIndex(pageNumber);
	qDebug() << "index:" << i;
	int index = findIndex(pageNumber); 
	if (index == -1)
	{
		return -1;

	}
	int entry = entries[index];
	if (isProtected(entry) && write) {
		return -2;
	}
	if (!isPresent(entry)) {
		return -3; //page fault
	}
	entry = setBit(entry, 8, true); //Referenced
	if (write) {
		entry = setBit(entry, 7, true); //Modified
	}
	entries[index] = entry;
	//Adresa fizica
	int pageFrameNr = getPageFrameNumber(entry);
	int physicalAddress = pageFrameNr * pageSize + offset;
	return physicalAddress;

}

void TLB::setMapping(int pageNumber, int frameNumber, bool protection) {
	//if (pageNumber < 0 || pageNumber >= totalEntries) {
		//throw std::out_of_range("Numarul paginii nu este corect");
	//}
	if (fifoQueue.size() == totalEntries)
	{
		int indexDelete = fifoQueue.front();
		fifoQueue.pop_front();
		entries[indexDelete] = 0; 
	}
	int indexNew = -1;
	for (int i = 0; i < totalEntries; ++i) {
		if (std::find(fifoQueue.begin(), fifoQueue.end(), i) == fifoQueue.end()) {
			indexNew = i; 
			break;
		}
	}
	int entry = 0;
	entry = setPageNumber(entry, pageNumber);
	entry = setPageFrameNumber(entry, frameNumber);
	entry = setBit(entry, 5, true); 
	entry = setBit(entry, 6, protection); 

	entries[indexNew] = entry;
	fifoQueue.push_back(indexNew); 
	//qDebug() << ":" << index;

}

std::vector<int> TLB::getEntryValues(int pageNumber) {
	int index = findIndex(pageNumber);
	qDebug() << "TLB index:" << index;
	if (index == -1) {
		//throw std::out_of_range("Page not found in TLB");
		std::vector<int> values(6);
		values[0] = -1;
		values[1] = -1;
		values[2] = -1;
		values[3] = -1;
		values[4] = -1;
		values[5] = -1;
		//return -1;
	}

	int entry = entries[index];
	std::vector<int> values(6);

	values[0] = getPageNumber(entry);
	values[1] = isReferenced(entry);
	values[2] = isModified(entry);
	values[3] = isProtected(entry);
	values[4] = isPresent(entry);
	values[5] = getPageFrameNumber(entry);

	return values;
}

int TLB::findIndex(int pageNumber)
{
	for (int i = 0; i < totalEntries; ++i) {
		int entry = entries[i];
		if (getPageNumber(entry) == pageNumber) {
			return i;
		}
	}
	return -1; //daca nu se gaseste
}

void TLB::update(int pageNumber, bool write, bool absent)
{
	int index = findIndex(pageNumber);
	if (index != -1)
	{

		if (absent == 1)
		{
			int entry = entries[index];
			setBit(entry, 5, 0);
		}
		else {
			int entry = entries[index];
			setBit(entry, 8, 1);
			if (write)
			{
				setBit(entry, 7, 1);
			}
		}

	}
}

