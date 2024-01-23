#pragma once

#include <iostream>;
#include <string>
#include <fstream>;
#include <tuple>

#include "base64.h";
#include "BaseTools.h";
#include "Color.h";

typedef unsigned char BYTE;

//QOI TAGS https://qoiformat.org/qoi-specification.pdf
int MAGIC_WORD = 0x66696f71;
int QOI_OP_RGB = 0b11111110;
int QOI_OP_RGBA = 0b11111111;
int QOI_OP_INDEX = 0b00000000;
int QOI_OP_LUMA = 0b10000000;
int QOI_OP_RUN = 0b11000000;
int QOI_OP_DIFF = 0b01000000;

int QOI_HEADER_SIZE = 14;
int QOI_ENDING[] = { 0, 0, 0, 0, 0, 0, 0, 1 };

static std::tuple<BYTE*, int>QoiEncode(BYTE* data, int width, int height, int channels, int colorspace) {
	int last_color = width * height * channels - channels;
	int max_size = width * height * (channels + 1) + QOI_HEADER_SIZE + std::size(QOI_ENDING);
	BYTE* bytes = new BYTE[max_size];

	Color previous_color(0, 0, 0, 255);
	Color* seen_colors = new Color[64];

	int index = 0;

	auto WriteByte = [&](int value) {
		bytes[index++] = value;
	};

	auto Write32 = [&](int value) {
		WriteByte((value & 0x000000ff) >> 0);
		WriteByte((value & 0x0000ff00) >> 8);
		WriteByte((value & 0x00ff0000) >> 16);
		WriteByte((value & 0xff000000) >> 24);
	};

	auto ReadByte = [&](int index) {
		return data[index];
	};

	Write32(MAGIC_WORD);
	Write32(width);
	Write32(height);
	WriteByte(channels);
	WriteByte(colorspace);

	int run = 0;

	for (int offset = 0; offset <= last_color; offset += channels){
		Color this_color(
			ReadByte(0 + offset),
			ReadByte(1 + offset),
			ReadByte(2 + offset),
			channels == 4 ? ReadByte(3 + offset) : previous_color.a
		);

		if (this_color == previous_color){
			run += 1;
			if (offset == last_color){
				WriteByte(QOI_OP_RUN | run - 1);
				continue;
			}

			if (run == 62) {
				WriteByte(QOI_OP_RUN | run - 1);
				run = 0;
			}
			continue;
		}

		if (run != 0) {
			WriteByte(QOI_OP_RUN | run - 1);
			run = 0;
		}

		Color color_difference = this_color - previous_color;
		previous_color = this_color;

		int hash = this_color.GetHash();
		if (seen_colors[hash] == this_color) {
			WriteByte(QOI_OP_INDEX | hash);
			continue;
		}
		seen_colors[hash] = this_color;

		if (color_difference.a != 0) {
			WriteByte(QOI_OP_RGBA);
			Write32(this_color.To32BitValue());
			continue;
		}

		int dr_dg = color_difference.r - color_difference.g;
		int db_dg = color_difference.b - color_difference.g;

		if (color_difference.r >= -2 and color_difference.r <= 1 and
				color_difference.g >= -2 and color_difference.g <= 1 and
				color_difference.b >= -2 and color_difference.b <= 1
			) {
			WriteByte(
				QOI_OP_DIFF |
				((color_difference.b + 2) << 0) |
				((color_difference.g + 2) << 2) |
				((color_difference.r + 2) << 4)
			);
			continue;
		}

		if (color_difference.g >= -32 and color_difference.g <= 31 and
				dr_dg >= -8 and dr_dg <= 7 and
				db_dg >= -8 and db_dg <= 7
			) {
			WriteByte(QOI_OP_LUMA | (color_difference.g + 32));
			WriteByte(
				((dr_dg + 8) << 4) +
				((db_dg + 8) << 0)
			);
			continue;
		}

		WriteByte(QOI_OP_RGB);
		WriteByte(this_color.r);
		WriteByte(this_color.g);
		WriteByte(this_color.b);
	}

	for (int i : QOI_ENDING) {
		WriteByte(i);
	}
	BYTE* cutted_buffer = CutBuffer(bytes, index);

	delete[] bytes;
	return { cutted_buffer, index};
}

static inline std::tuple<std::string, int> QoiEncodeToReadableString(BYTE* data, int width, int height, int channels, int colorspace) {
	auto [compressed_buffer, size] = QoiEncode(data, width, height, channels, colorspace);
	std::string encoded_string = base64_encode(compressed_buffer, size);

	delete[] compressed_buffer;
	return { encoded_string, size};
}