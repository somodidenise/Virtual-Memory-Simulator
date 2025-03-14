#include "VirtualAddressSpace.h"
#include <cstdlib> // For rand()
#include <ctime>   // For seeding rand()
#include <stdexcept>
VirtualAddressSpace::VirtualAddressSpace() : size(4096), addresses(size, 0) {
    initializeRandom(true);
}
void VirtualAddressSpace::initializeRandom(bool seedRandom) {
    if (seedRandom) {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }
    for (size_t i = 0; i < size; ++i) {
        addresses[i] = static_cast<unsigned char>(std::rand() % 256);
    }
}
const std::vector<unsigned char>& VirtualAddressSpace::getAddresses() const {
    return addresses;
}
unsigned char VirtualAddressSpace::getAddress(size_t index) const {
    if (index < size) {
        return addresses[index];
    }
    throw std::out_of_range("Index out of bounds");
}
