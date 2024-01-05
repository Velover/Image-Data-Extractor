#include <iostream>;
#include <fstream>;
#include <sstream>;
#include <string>;
#include <filesystem>;

#include "opencv2/highgui.hpp";
#include "opencv2/opencv.hpp";

#include "Qoi.h";
#include "BaseTools.h";
#include "LuaHandler.h";

typedef std::vector<std::tuple<cv::Mat, std::string>> Images;
namespace fs = std::filesystem;

const std::string IMAGE_FOLDER_PATH = "./Images";
const std::string OUT_FOLDER_PATH = "./Out";
const int MAX_IMAGE_SIZE = 1024;

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

void NormalizeImage(cv::Mat& image) {
	//max size is 1024x1024 - it will resize the image to that range
	using namespace cv;
	Size image_size = image.size();
	double coeficient_width = double(image_size.width) / double(MAX_IMAGE_SIZE);
	double coeficient_height = double(image_size.height) / double(MAX_IMAGE_SIZE);

	if (coeficient_height <= 1 and coeficient_width <= 1) {
		return;
	}

	Size normalized_size;
	if (coeficient_width >= coeficient_height) {
		normalized_size = Size(MAX_IMAGE_SIZE, image_size.height / coeficient_width);
	}
	else {
		normalized_size = Size(image_size.width / coeficient_height, MAX_IMAGE_SIZE);
	}

	Mat resized_image;
	resize(image, resized_image, normalized_size);

	image = resized_image;
}

Images GetImages() {
	CheckFolders();
	Images images;
	for (auto& entry : fs::directory_iterator(IMAGE_FOLDER_PATH)) {
		if (!fs::is_regular_file(entry.path())) {
			continue;
		}
		cv::Mat image = cv::imread(entry.path().string(), cv::IMREAD_UNCHANGED);
		if (image.empty()) {
			continue;
		}
		NormalizeImage(image);
		images.push_back({ image, GetName(entry.path().string()) });
	}

	return images;
}

static BYTE* GetColors3(cv::Mat& image) {
	using namespace cv;
	const Size image_size = image.size();
	//creates buffer with 0's
	BYTE* image_buffer = new BYTE[image_size.width * image_size.height * 3](0);

	int index = 0;
	for (int y = 0; y < image_size.height; y++) {
		for (int x = 0; x < image_size.width; x++) {
			Vec3b& pixel = image.at<Vec3b>(Point(x, y));
			//hate this but the colorspace is in bgr
			uchar& r = pixel[2];
			uchar& g = pixel[1];
			uchar& b = pixel[0];

			image_buffer[index++] = r;
			image_buffer[index++] = g;
			image_buffer[index++] = b;
		}
	}
	//reads the image and writes it to the buffer and returns it
	return image_buffer;
}

static BYTE* GetColors4(cv::Mat& image) {
	using namespace cv;
	const Size image_size = image.size();
	BYTE* image_buffer = new BYTE[image_size.width * image_size.height * 4](0);

	int index = 0;
	for (int y = 0; y < image_size.height; y++) {
		for (int x = 0; x < image_size.width; x++) {
			Vec4b& pixel = image.at<Vec4b>(Point(x, y));
			//color space is brga

			uchar& r = pixel[2];
			uchar& g = pixel[1];
			uchar& b = pixel[0];
			uchar& a = pixel[3];

			image_buffer[index++] = r;
			image_buffer[index++] = g;
			image_buffer[index++] = b;
			image_buffer[index++] = a;
		}
	}

	return image_buffer;
}

BYTE* GetColors(cv:: Mat& image) {
	const int channels = image.channels();
	return channels == 4 ? GetColors4(image) : GetColors3(image);
}

bool CompressAndSaveImage(BYTE* color_buffer, cv::Mat& image, std::string& name) {
	const cv::Size image_size = image.size();
	const int channels = image.channels();
	auto [encoded_string, size] = QoiEncodeToReadableString(color_buffer, image_size.width, image_size.height, channels, 1);

	std::string compressed_string = CompressString(encoded_string); //base tools

	ContinueLog(name);
	ContinueLog("| MemorySize: ");
	ContinueLog(compressed_string.size());
	ContinueLog("| ImageSize: ");
	Log(image_size);

	SaveDataAtPath(OUT_FOLDER_PATH + "/" + name + ".txt", compressed_string);
	return true;
}

int main() {
	Log("The images width size bigger than 1024 will be rescalled");
	Log("Press Enter to start");
	std::cin.get();

	Images images = GetImages();

	bool initialisation_success = InitializeLua();
	if (not initialisation_success) {
		Log("Problem with lua files");
		return 0;
	}
	for (auto [image, name] : images) {
		BYTE* image_color_buffer = GetColors(image);
		bool success = CompressAndSaveImage(image_color_buffer, image, name);
		delete[] image_color_buffer;
	}
	CloseLua();
	Log("----------------");
	Log("All done! Press Enter To Close");
	std::cin.get();
	return 0;
}