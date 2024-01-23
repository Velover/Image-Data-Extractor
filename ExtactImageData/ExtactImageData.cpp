#include <iostream>;
#include <fstream>;
#include <sstream>;
#include <string>;
#include <filesystem>;
#include <thread>;
#include <vector>


#include "ImageProcessor.h";

namespace fs = std::filesystem;

static const std::string IMAGE_FOLDER_PATH = "./Images";
static const std::string OUT_FOLDER_PATH = "./Out";


void CheckFolders() {
	//Creates needed directories if needed
	const fs::path image_folder_path(IMAGE_FOLDER_PATH);
	const fs::path out_folder_path(OUT_FOLDER_PATH);
	if (!fs::is_directory(image_folder_path)) {
		fs::create_directory(image_folder_path);
	}
	if (!fs::is_directory(out_folder_path)) {
		fs::create_directory(out_folder_path);
	}
}

void ProcessImages() {
	const int amount_of_threads = 32;
	std::vector<std::thread> all_threads = {};
	int itteration = 0;
	for (auto& entry : fs::directory_iterator(IMAGE_FOLDER_PATH)) {
		if (!fs::is_regular_file(entry.path())) {
			continue;
		}
		//thread implementation
		int thread_index = itteration % amount_of_threads;
		if (itteration < amount_of_threads) {
			all_threads.push_back(std::thread(ProcessImage, entry.path().string(), OUT_FOLDER_PATH));
		}
		else {
			if (all_threads[thread_index].joinable()) {
				all_threads[thread_index].join();
			}
			all_threads[thread_index] = std::thread(ProcessImage, entry.path().string(), OUT_FOLDER_PATH);
		}

		itteration++;
	}

	for (int i = 0; i < all_threads.size(); i++) {
		if (all_threads[i].joinable()) {
			all_threads[i].join();
		}
	}
}

int main() {
	Log("The images width size bigger than 1024 will be rescalled");
	Log("Press Enter to start");

	CheckFolders();
	

	std::cin.get();
	int quality = -1;
	Log("Enter quality 0 - 100,  0 - worst, 100 - best");
	std::cin >> quality;
	
	quality = std::clamp(quality, 0, 100);
	float compression_quality = Map(static_cast<float>(quality), 0.f, 100.f, 1.f, .05f, true);

	bool double_encoding = true;
	Log("Can increase decode time by 10-20ms, in some cases will decrease the size down to 50%");
	Log("Double encoding [0] - false, [1] - true");
	std::cin >> double_encoding;

	InitializeProcessor(compression_quality, double_encoding);
	ProcessImages();

	Log("----------------");
	Log("All done! Press Enter To Close");
	std::cin.get();
	return 0;
}