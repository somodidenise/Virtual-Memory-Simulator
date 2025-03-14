#include "PhysicalMemory.h"
#include <iostream>
#include <vector>
#include <queue>
#include <list>
#include <stdexcept>
#include <QDebug>

PhysicalMemory::PhysicalMemory(int memorySize, int totalFrames, int frameSize, std::string replacementPolicy) : memorySize(memorySize), totalFrames(totalFrames), frameSize(frameSize), replacementPolicy(replacementPolicy){
    memoryFrames = memorySize / frameSize;
	addresses.resize(memoryFrames);
	for (int i = 0; i < memoryFrames; i++)
	{
		addresses[i].resize(frameSize, 0);
	}
	secondaryAddresses.resize(totalFrames);
	for (int i = 0; i < totalFrames; i++)
	{
		secondaryAddresses[i].resize(frameSize, 0);
	}
	for (int i = 0; i < memoryFrames; i++)
	{
		availableFrames.push(i);
	}
}

void PhysicalMemory::write(int frameNumber, int offset, unsigned char data) {
    if (frameNumber >= 0 && frameNumber < static_cast<int>(addresses.size()) &&
        offset >= 0 && offset < frameSize) {
        addresses[frameNumber][offset] = data;
    }
    else {
        throw std::out_of_range("Invalid frame number or offset for write operation.");
    }
}

unsigned char PhysicalMemory::read(int frameNumber, int offset) {
    if (frameNumber >= 0 && frameNumber < static_cast<int>(addresses.size()) &&
        offset >= 0 && offset < frameSize) {
        return addresses[frameNumber][offset];
    }
    //else {
      //  throw std::out_of_range("Invalid frame number or offset for read operation.");
    //}
}

void PhysicalMemory::freeFrame(int frameNumber) { //modified bool
    if (frameNumber >= 0 && frameNumber < static_cast<int>(addresses.size())) {
        //if (modified) {
          //  secondaryAddresses[frameNumber] = addresses[frameNumber];
        //}
        std::fill(addresses[frameNumber].begin(), addresses[frameNumber].end(), 0);
        availableFrames.push(frameNumber);
    }
    else {
        throw std::out_of_range("Invalid frame number for freeFrame operation.");
    }
}

int PhysicalMemory::getNextAvailableFrame() {
    if (!availableFrames.empty()) {
        int frame = availableFrames.front();
        availableFrames.pop();
        return frame;
    }
    else {
        throw std::runtime_error("No available frames in memory.");
    }
}

bool PhysicalMemory::isFull() const {
    qDebug() << "Available frames size:" << availableFrames.size();
    return availableFrames.empty();
}

//inainte de apelare verificam isFull
int PhysicalMemory::addPageFrame(int virtualPageNumber) {
    int frameNumber = getNextAvailableFrame();
    addresses[frameNumber] = secondaryAddresses[virtualPageNumber];
    //secondaryAddresses[virtualPageNumber].clear();  
    if (replacementPolicy == "FIFO") {
        fifoQueue.push(frameNumber);
    }
    else if (replacementPolicy == "LRU") {
        lruList.push_back(frameNumber);
    }
    return frameNumber;
}

int PhysicalMemory::removePageFrame() { //modified
    int frameToRemove;
    qDebug() << "replacement:" << replacementPolicy;

    if (replacementPolicy == "FIFO") {
        if (fifoQueue.empty()) {
            throw std::runtime_error("No page frame to remove.");
        }
        frameToRemove = fifoQueue.front();
        fifoQueue.pop();
    }
    else if (replacementPolicy == "LRU") {
        if (lruList.empty()) {
            throw std::runtime_error("No page frame to remove.");
        }
        frameToRemove = lruList.front();
        lruList.pop_front();
    }
    else {
        throw std::runtime_error("Invalid replacement policy.");
    }
    availableFrames.push(frameToRemove);

    return frameToRemove;
}

void PhysicalMemory::updateLRU(int frameNumber) {
    if (replacementPolicy == "LRU") {
        lruList.remove(frameNumber);  // Remove if already present
        lruList.push_back(frameNumber);  // Add to the end (most recently used)
    }
}

void PhysicalMemory::writeToSecondaryMem(int frameToRemove) {
    secondaryAddresses[frameToRemove] = addresses[frameToRemove];
}