#pragma once
#include <iostream>;
#include <vector>;
#include <deque>;

#include "opencv2/highgui.hpp";
#include "opencv2/opencv.hpp";

#include "Color.h"
#include "Qoi.h";
#include "BaseTools.h";
#include "Binary.h";
#include "ImageUtilities.h";
#include "Huffman.h";

Matrix<double> LUMINANCE_QUANTASATION_TABLE({
	16, 11, 10, 16, 24, 40, 51, 61,
	12, 12, 14, 19, 26, 58, 60, 55,
	14, 13, 16, 24, 40, 57, 69, 56,
	14, 17, 22, 29, 51, 87, 80, 62,
	18, 22, 37, 56, 68, 109, 103, 77,
	24, 35, 55, 64, 81, 104, 113, 92,
	49, 64, 78, 87, 103, 121, 120, 101,
	72, 92, 95, 98, 112, 100, 103, 99
	});

Matrix<double> CHROMINANCE_QUANTASATION_TABLE({
	17, 18, 24, 47, 99, 99, 99, 99,
	18, 21, 26, 66, 99, 99, 99, 99,
	24, 26, 56, 99, 99, 99, 99, 99,
	47, 66, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99,
	99, 99, 99, 99, 99, 99, 99, 99
	});

const std::string JPEG_TAG = "JPG_";
const int MAX_IMAGE_SIZE = 1024;
static enum BlockTypes {
	luminocity,
	chrominance
};

static union{
	int i;
	float f;
}converter;

static bool second_huffman_encoding = false;

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

static ColorData GetColors3(cv::Mat& image) {
	using namespace cv;
	const Size image_size = image.size();
	//creates buffer with 0's
	ColorData image_buffer;

	int index = 0;
	for (int y = 0; y < image_size.height; y++) {
		for (int x = 0; x < image_size.width; x++) {
			Vec3b& pixel = image.at<Vec3b>(Point(x, y));
			//hate this but the colorspace is in bgr
			uchar& r = pixel[2];
			uchar& g = pixel[1];
			uchar& b = pixel[0];

			image_buffer.push_back(r);
			image_buffer.push_back(g);
			image_buffer.push_back(b);
			//requires to have 4 components
			image_buffer.push_back(255);
		}
	}
	//reads the image and writes it to the buffer and returns it
	return image_buffer;
}

static ColorData GetColors4(cv::Mat& image) {
	using namespace cv;
	const Size image_size = image.size();
	ColorData image_buffer;

	int index = 0;
	for (int y = 0; y < image_size.height; y++) {
		for (int x = 0; x < image_size.width; x++) {
			Vec4b& pixel = image.at<Vec4b>(Point(x, y));
			//color space is brga

			uchar& r = pixel[2];
			uchar& g = pixel[1];
			uchar& b = pixel[0];
			uchar& a = pixel[3];

			image_buffer.push_back(r);
			image_buffer.push_back(g);
			image_buffer.push_back(b);
			image_buffer.push_back(a);
		}
	}

	return image_buffer;
}

ColorData GetColors(cv::Mat& image) {
	const int channels = image.channels();
	return channels == 4 ? GetColors4(image) : GetColors3(image);
}

std::string EncodeBlocks(std::vector<ColorData>& blocks, BlockTypes type, bool second_encoding = false) {
	std::deque<int> data;
	for (int i = 0; i < blocks.size(); i++) {
		ColorData& block = blocks[i];
		Matrix<ColorValue> matrix = IU_To8x8Matix(block);
		Matrix<double> double_matrix = matrix.Convert<double>();
		double_matrix.Offset(-128.);
		Matrix<double> dct_matrix = double_matrix.MatrixToDCT();

		if (type == luminocity) {
			dct_matrix.Quantize(LUMINANCE_QUANTASATION_TABLE);		
		}
		else {
			dct_matrix.Quantize(CHROMINANCE_QUANTASATION_TABLE);
		}
		dct_matrix.Round();
		Matrix<int> int_dct_matrix = dct_matrix.Convert<int>();
		std::deque<int> diagonal_array = int_dct_matrix.ToDiagonalIntArray();
		for (int i = 0; i < diagonal_array.size(); i++) {
			data.push_back(diagonal_array[i]);
		}
	}

	std::string encoded_string = HuffmanEncodeInt(data);
	if (not second_encoding) {
		return encoded_string;
	}

	std::deque<int> character_array = StringToU_INTArray(encoded_string);
	std::string double_encoded_string = HuffmanEncodeInt(character_array);
	return double_encoded_string;
}

void InitializeProcessor(float quality, bool second_encoding) {
	converter.f = quality;
	second_huffman_encoding = second_encoding;

	LUMINANCE_QUANTASATION_TABLE.Multiply(converter.f);
	CHROMINANCE_QUANTASATION_TABLE.Multiply(converter.f);
}

void ProcessImage(std::string image_path, std::string save_forder_path) {
	cv::Mat image = cv::imread(image_path, cv::IMREAD_UNCHANGED);
	if (image.empty()) {
		return;
	}
	NormalizeImage(image);

	cv::Size cv_image_size = image.size();
	ImageSize image_size(cv_image_size.width, cv_image_size.height);
	ImageSize scaled_image_size(cv_image_size.width / 2, cv_image_size.height / 2);
	ColorData image_data = GetColors(image);

	ColorData ycbcr_data = IU_ImageRGBAToYCbCr(image_data, image_size);
	std::vector<ColorData> components = IU_Decompose(ycbcr_data, 3);

	ColorData y = components[0];
	ColorData cb = components[1];
	ColorData cr = components[2];

	ColorData scaled_cb = IU_ScaleComponent(cb, image_size, scaled_image_size);
	ColorData scaled_cr = IU_ScaleComponent(cr, image_size, scaled_image_size);
	std::vector<ColorData> y_blocks = IU_SplitComponentWithFill(y, image_size, ImageSize(8, 8), 255);
	std::vector<ColorData> cb_blocks = IU_SplitComponentWithFill(scaled_cb, scaled_image_size, ImageSize(8, 8), 255);
	std::vector<ColorData> cr_blocks = IU_SplitComponentWithFill(scaled_cr, scaled_image_size, ImageSize(8, 8), 255);

	std::string y_encoded = EncodeBlocks(y_blocks, luminocity, second_huffman_encoding);
	std::string cb_encoded = EncodeBlocks(cb_blocks, chrominance, second_huffman_encoding);
	std::string cr_encoded = EncodeBlocks(cr_blocks, chrominance, second_huffman_encoding);

	U_INT32 y_size = y_encoded.size();
	U_INT32 cb_size = cb_encoded.size();
	U_INT32 cr_size = cr_encoded.size();

	//16 bits for x, 16 bits for y;
	U_INT32 image_size_number = AddEndBits(0, image_size.X, 0);
	image_size_number = AddEndBits(image_size_number, image_size.Y, 16);

	std::string image_start_string = JPEG_TAG;
	image_start_string += Int32ToString(second_huffman_encoding);
	image_start_string += Int32ToString(image_size_number);
	image_start_string += Int32ToString(converter.i);
	image_start_string += Int32ToString(y_size);
	image_start_string += Int32ToString(cb_size);
	image_start_string += Int32ToString(cr_size);

	std::string saved_data = image_start_string + y_encoded + cb_encoded + cr_encoded;
	SaveDataAtPath(save_forder_path + "/" + GetName(image_path) + ".txt", ToBase64(saved_data));
	//do something with image
}