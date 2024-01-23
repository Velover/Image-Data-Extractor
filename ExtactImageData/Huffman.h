#pragma once
#include <unordered_map>;
#include <array>;
#include <vector>;
#include <deque>;
#include <unordered_map>
#include <map>;
#include <string>;

#include "Binary.h";
#include "Compactor.h"

CU_INT32 null = 0x9000000f;
CU_INT32 BIT_31_NUMBER = 2147483648;
CU_INT16 BIT_15_NUMBER = 32768;

struct Branch {
	int value;
	unsigned int q;
	unsigned int path = 0;
	Branch* left_branch;
	Branch* right_branch;
	Branch* parent;
	bool parent_set = 0;
};

struct HuffmanTableElement {
	int value;
	unsigned int path_length;
};

struct ReconstructedTableElement {
	U_INT32 path;
	U_INT32 path_length;
};

typedef std::unordered_map<int, ReconstructedTableElement*> ReconstructedHuffmanTable;
typedef std::unordered_map<U_INT32, HuffmanTableElement*> HuffmanTable;

HuffmanTable ToHuffmanTable(std::deque<int>& values) {
	std::unordered_map<int, unsigned int> quantities;
	for (int& value : values) {
		quantities[value] += 1;
	}

	std::deque<Branch*> original_data;
	std::deque<Branch*> branches;
	for (auto& value_tuple : quantities) {
		int value = value_tuple.first;
		unsigned int quantity = value_tuple.second;
		Branch* branch = new Branch;
		branch->q = quantity;
		branch->value = value;
		original_data.push_back(branch);
		branches.push_back(branch);
	}

	if (original_data.size() == 1) { //fix the issue with a couple 
		HuffmanTable huffman_table;
		HuffmanTableElement* element = new HuffmanTableElement;
		element->value = original_data[0]->value;
		element->path_length = 1;
		huffman_table[0] = element;
		return huffman_table;
	}

	//sort ///////////
	for (int i = 1; i < branches.size(); i++) {
		for (int v = i; v > 0; v--) {
			Branch* branch1 = branches[v];
			Branch* branch2 = branches[v - 1];
			if (branch1->q > branch2->q) {
				continue;
			}
			branches[v - 1] = branch1;
			branches[v] = branch2;
		}
	}

	std::deque<Branch*> temp;
	int i = 0;
	while (branches.size() > 1) {
		Branch* branch1 = branches[0];
		Branch* branch2 = branches[1];
		branches.pop_front();
		branches.pop_front();

		Branch* branch = new Branch;
		temp.push_back(branch);
		branch->q = branch1->q + branch2->q;
		branch->value = null;
		branch1->parent = branch;
		branch2->parent = branch;
		branch1->parent_set = 1;
		branch2->parent_set = 1;
		branch1->path = 0;
		branch2->path = 1;

		std::deque<Branch*>::iterator itterator = branches.begin();
		if (branches.size() == 0) {
			branches.insert(itterator, branch);
			break;
		}

		for (int i = 0; i < branches.size(); i++) {
			Branch* comparison_brunch = branches[i];
			if ((comparison_brunch->q <= branch->q) and (i != branches.size() - 1)) {
				continue;
			}
			std::deque<Branch*>::iterator itterator = branches.begin();
			branches.insert(itterator + i, branch);
			break;
		}
	}

	HuffmanTable huffman_table;
	for (int i = 0; i < original_data.size(); i++) {
		std::string string_path = "";
		Branch* current_value = original_data[i];
		while (current_value->parent_set == 1) {
			string_path += std::to_string(current_value->path);
			current_value = current_value->parent;
		}
		CU_INT32 path_number = std::stoi(string_path.c_str(), nullptr, 2);
		CU_INT32 path_length = string_path.length();
		HuffmanTableElement* element = new HuffmanTableElement;
		element->value = original_data[i]->value;
		element->path_length = path_length;
		huffman_table[path_number] = element;
	}

	//garbage collector
	for (int i = 0; i < original_data.size(); i++) {
		delete original_data[i];
	}
	for (int i = 0; i < temp.size(); i++) {
		delete temp[i];
	}

	return huffman_table;
}

std::string HuffmanEncodeInt(std::deque<int>& data) {
	HuffmanTable huffman_table = ToHuffmanTable(data);
	BinaryCompactor compactor;

	U_INT32 path_total_length = 0;
	U_INT32 huffman_table_size = 0;

	ReconstructedHuffmanTable reconstructed_huffman_table;

	for (auto& element : huffman_table) {
		huffman_table_size++;

		U_INT32 path = element.first;
		HuffmanTableElement* value_tuple = element.second;
		int value = value_tuple->value;
		U_INT32 path_length = value_tuple->path_length;

		ReconstructedTableElement* reconstructed_element = new ReconstructedTableElement;
		reconstructed_element->path = path;
		reconstructed_element->path_length = path_length;

		reconstructed_huffman_table[value] = reconstructed_element;
	}

	for (int& value : data) {
		ReconstructedTableElement* path_tuple = reconstructed_huffman_table[value];
		path_total_length += path_tuple->path_length;
	}

	compactor.Add(path_total_length, 32);
	compactor.Add(huffman_table_size, 32);

	for (auto& element : huffman_table) {
		U_INT32 path = element.first;
		HuffmanTableElement* value_tuple = element.second;

		compactor.Add(value_tuple->value + BIT_15_NUMBER, 16);
		compactor.Add(value_tuple->path_length, 32);
		compactor.Add(path, value_tuple->path_length);
	}

	for (int& value : data) {
		ReconstructedTableElement* path_tuple = reconstructed_huffman_table[value];
		CU_INT32 path = path_tuple->path;
		CU_INT32 path_length = path_tuple->path_length;
		compactor.Add(path, path_length);
	}

	//garbage collector
	for (auto& element : huffman_table) {
		delete element.second;
	}

	for (auto& element : reconstructed_huffman_table) {
		delete element.second;
	}

	return compactor.Finish();
}
