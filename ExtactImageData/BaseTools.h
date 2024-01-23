#pragma once
#include <deque>

#include "base64.h"
#include "Binary.h"

template <typename Buffer>
static Buffer* CutBuffer(Buffer* buffer, int length) {
	Buffer* return_buffer = new Buffer[length];
	for (int i = 0; i < length; i++) {
		return_buffer[i] = buffer[i];
	}

	return return_buffer;
}

template <typename Message> static void Log(Message message) {
	std::cout << message << std::endl;
}

template <typename Message> static void FastLog(Message message) {
	std::cout << message << "\n";
}

template <typename Message> static void ContinueLog(Message message) {
	std::cout << message;
}

static std::string IntToHex(int value)
{
	std::ostringstream stream;
	stream << std::hex << value;
	return stream.str();
}

static std::string GetName(std::string const& path)
{
	return path.substr(path.find_last_of("/\\") + 1);
}

static void SaveDataAtPath(std::string path, std::string data) {
	std::fstream file;
	file.open(path, std::ios::out);
	if (!file.is_open()) {
		Log("File " + path + " wasn't saved");
		return;
	}

	file << data;
	file.close();
	Log("Saved file " + path);
}


static U_CHAR* StringToU_CHARArray(std::string& string) {
	CU_INT32 string_size = string.size();
	U_CHAR* data = new U_CHAR[string_size];
	for (int i = 0; i < string_size; i++) {
		data[i] = static_cast<CU_CHAR>(string[i]);
	}

	return data;
}

static std::string ToBase64(std::string& string) {
	U_CHAR* data = StringToU_CHARArray(string);
	std::string encoded_string = base64_encode(data, string.size());
	delete data;
	return encoded_string;
}

static std::deque<int> StringToU_INTArray(std::string& string) {
	std::deque<int> data;
	for (int i = 0; i < string.size(); i++) {
		U_CHAR character = static_cast<U_CHAR>(string[i]);
		data.push_back(character);
	}
	return data;
}

float Map(float value, float min, float max, float output_min, float output_max, bool keep_in_range = false) {
	float difference_input = max - min;
	float difference_output = output_max - output_min;
	float difference = value - min;

	float coefizient = difference_output / difference_input;
	float return_value = output_min + difference * coefizient;
	return keep_in_range ? std::clamp(return_value, std::min(output_min, output_max), std::max(output_min, output_max)) : return_value;
}