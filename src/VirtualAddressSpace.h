#pragma once
#include <vector>
class VirtualAddressSpace
{
private:
	int size; //4 kb = 4096 bytes
public:
	VirtualAddressSpace();
	std::vector<unsigned char> addresses;
	void initializeRandom(bool seedRandom);
	const std::vector<unsigned char>& getAddresses() const;
	unsigned char getAddress(size_t index) const;
};

