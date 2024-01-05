#pragma once

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