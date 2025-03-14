#include "VirtualMemoryManager.h"
#include <iostream>
VirtualMemoryManager::VirtualMemoryManager(int memorySize, int pageSize, int totalFrames, std::string replacementPolicy):
			memorySize(memorySize), pageSize(pageSize), totalFrames(totalFrames), replacementPolicy(replacementPolicy), 
			physicalMemory(memorySize, totalFrames, pageSize, replacementPolicy), pageTable(totalFrames, pageSize), tlb(pageSize),
            virtualAddressSpace()
{
    //copiem datele din virtualAddressSpace in memoria secundara
    for (int i = 0; i < totalFrames; i++)
    {
        for (int j = 0; j < pageSize; j++)
        {   
            physicalMemory.secondaryAddresses[i][j] = virtualAddressSpace.addresses[i * pageSize + j];
        }
    }

}
int VirtualMemoryManager::translateAddress(int readAddress) {
    int pageNumber = readAddress / pageSize;
    int offset = readAddress % pageSize;
    int returnAddress = (pageNumber << 12) | offset;
    return returnAddress;
}