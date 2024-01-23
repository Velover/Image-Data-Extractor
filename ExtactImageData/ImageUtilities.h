#pragma once
#include <iostream>;
#include <deque>;
#include <array>;
#include <vector>;
#include <tuple>;

#include "BaseTools.h";
#include "Binary.h";
#include "Color.h";
#include "Matrix.h";

typedef U_CHAR ColorValue;
typedef std::deque<ColorValue> ColorData;

struct ImageSize{
	int X;
	int Y;
	ImageSize(int x, int y) {
		this->X = x;
		this->Y = y;
	}
};

std::array<ColorValue, 3>RGBToYCbCr(ColorValue& r, ColorValue& g, ColorValue& b) {
	ColorValue Y = .299 * static_cast<double>(r) + .587 * static_cast<double>(g) + .114 * static_cast<double>(b);
	ColorValue Cb = -.169 * static_cast<double>(r) -.331 * static_cast<double>(g) + .5 * static_cast<double>(b) + 128.;
	ColorValue Cr = .5 * static_cast<double>(r) - .419 * static_cast<double>(g) - .081 * static_cast<double>(b) + 128.;

	return { Y, Cb, Cr };
}

std::vector<ColorData> IU_Decompose(ColorData image_data, U_CHAR amount_of_components) {
	std::vector<ColorData> components = {};
	for (int i = 0; i < amount_of_components; i++) {
		components.push_back({});
	}

	for (int i = 0; i < image_data.size() / amount_of_components; i++) {
		U_INT32 index = i * amount_of_components;
		for (int component_index = 0; component_index < amount_of_components; component_index++) {
			ColorData& component = components[component_index];
			ColorValue color = image_data[index + component_index];
			component.push_back(color);
		}
	}

	return components;
}

ColorData IU_Compose_4(ColorData& data_1, ColorData& data_2, ColorData& data_3, ColorData& data_4) {
	ColorData image_data;

	U_INT32 size = data_1.size();
	for (int i = 0; i < size; i++) {
		image_data.push_back(data_1[i]);
		image_data.push_back(data_2[i]);
		image_data.push_back(data_3[i]);
		image_data.push_back(data_4[i]);
	}
}

ColorData IU_Compose_3(ColorData& data_1, ColorData& data_2, ColorData& data_3) {
	ColorData image_data;

	U_INT32 size = data_1.size();
	for (int i = 0; i < size; i++) {
		image_data.push_back(data_1[i]);
		image_data.push_back(data_2[i]);
		image_data.push_back(data_3[i]);
	}
}

ColorData IU_CreateImageData(ImageSize size, ColorValue r, ColorValue g, ColorValue b, ColorValue a) {
	ColorData image_data;
	for (int i = 0; i < size.Y * size.X; i++) {
		image_data.push_back(r);
		image_data.push_back(g);
		image_data.push_back(b);
		image_data.push_back(a);
	}

	return image_data;
}

std::vector<ColorData> IU_SplitComponentWithFill(ColorData& color_data, ImageSize original_size, ImageSize desired_size, ColorValue default_value = 0) {
	std::vector<ColorData> blocks = {};
	int awaible_height = original_size.Y;
	for (int y = 0; y < original_size.Y; y += desired_size.Y) {
		int awaible_width = original_size.X;
		for (int x = 0; x < original_size.X; x += desired_size.X) {
			ColorData block = {};
			for (int v = 0; v < desired_size.Y; v++) {
				for (int u = 0; u < desired_size.X; u++) {
					int index = ((y + v) * original_size.X + (x + u));
					if ((y + v) >= original_size.Y or (x + u) >= original_size.X or
						(y + v) < 0 or (x + u) < 0) {
						block.push_back(default_value);
						continue;
					}
					ColorValue color = color_data[index];
					block.push_back(color);
				}
			}
			blocks.push_back(block);
		}
	}

	return blocks;
}

ColorData IU_ImageRGBAToYCbCr(ColorData& image_data, ImageSize size) {
	ColorData YCbCr;
	for (int y = 0; y < size.Y; y++) {
		for (int x = 0; x < size.X; x++) {
			int index = (y * size.X + x) * 4;

			ColorValue r = image_data[index];
			ColorValue g = image_data[index + 1];
			ColorValue b = image_data[index + 2];

			std::array<ColorValue, 3> converted_values = RGBToYCbCr(r, g, b);
			YCbCr.push_back(converted_values[0]);
			YCbCr.push_back(converted_values[1]);
			YCbCr.push_back(converted_values[2]);
		}
	}
	return YCbCr;
}

ColorData IU_ScaleComponent(ColorData& _color_data, ImageSize _image_size, ImageSize desired_size) {
	int width = desired_size.X;
	int height = desired_size.Y;
	double coefizient_x = static_cast<double>(_image_size.X) / static_cast<double>(desired_size.X);
	double coefizient_y = static_cast<double>(_image_size.Y) / static_cast<double>(desired_size.Y);
	ColorData image_data;
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int _y = y * coefizient_y;
			int _x = x * coefizient_x;
			int _index = _y * _image_size.X + _x;
			ColorValue& color = _color_data[_index];
			image_data.push_back(color);
		}
	}
	return image_data;
}

Matrix<ColorValue> IU_To8x8Matix(ColorData& color_data) {
	std::array<ColorValue, 64> array;
	for (int i = 0; i < 64; i++) {
		array[i] = color_data[i];
	}
	Matrix<ColorValue> matrix(array);

	return matrix;
}