#pragma once

#include "deque";
#include "string";
#include "vector";

#include "Binary.h";

class BinaryCompactor {
private:
	U_INT32 CurrentNumber = 0;
	U_INT32 CurrentBit = 0;
	std::deque<U_INT32> NumberArray;
public:
	BinaryCompactor() {};
	~BinaryCompactor() {};

	void Add(CU_INT32 value, CU_INT32 size) {
		CU_INT32 bit_index = CurrentBit % 32;
		CU_INT32 awaible_bits = INT_SIZE_BITS - bit_index;

		if (static_cast<int>(awaible_bits) - static_cast<int>(size) < 0) {
			CU_INT32 number_2_size = size - awaible_bits;
			CU_INT32 number_2 = ExtractBits(value, awaible_bits, size - awaible_bits);
			this->CurrentNumber = AddEndBits(this->CurrentNumber, value, bit_index);
			this->CurrentBit += awaible_bits;
			this->NumberArray.push_back(this->CurrentNumber);
			this->CurrentNumber = 0;
			this->Add(number_2, number_2_size);
			return;
		}

		CU_INT32 snapped_bit = this->CurrentBit - bit_index;
		this->CurrentNumber = AddEndBits(this->CurrentNumber, value, bit_index);
		this->CurrentBit += size;
		if (this->CurrentBit - snapped_bit < 32) {
			return;
		}
		this->NumberArray.push_back(this->CurrentNumber);
		this->CurrentNumber = 0;
	}

	std::string Finish() {
		CU_INT32 bit_index = CurrentBit % 32;
		if (bit_index != 0) {
			this->NumberArray.push_back(this->CurrentNumber);
		}

		std::string return_string = "";
		for (int i = 0; i < this->NumberArray.size(); i++) {
			const U_INT32 number = this->NumberArray[i];
			return_string += Int32ToString(number);
		}

		return return_string;
	}

};