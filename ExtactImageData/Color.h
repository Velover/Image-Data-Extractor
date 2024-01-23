#pragma once
#include <iostream>
#include <sstream>

#include "Binary.h"
#include "BaseTools.h"

class Color {
public:
	U_CHAR r;
	U_CHAR g;
	U_CHAR b;
	U_CHAR a;

	friend Color operator-(Color& color_1, Color& color_2) {
		return Color(
			color_1.r - color_2.r,
			color_1.g - color_2.g,
			color_1.b - color_2.b,
			color_1.a - color_2.a
		);
	}

	friend bool operator==(Color& color_1, Color& color_2) {
		return
			color_1.r == color_2.r &&
			color_1.g == color_2.g &&
			color_1.b == color_2.b &&
			color_1.a == color_2.a;
	}

	friend std::ostream& operator<<(std::ostream& out, Color const& color) {
		out << "[";

		out << color.r << ", ";
		out << color.g << ", ";
		out << color.b << ", ";
		out << color.a << "]";

		return out;
	}

	Color() {
		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 0;
	}

	Color(U_INT32 color_value) {
		U_CHAR r = (0x000000ff & color_value) >> 0;
		U_CHAR g = (0x0000ff00 & color_value) >> 8;
		U_CHAR b = (0x00ff0000 & color_value) >> 16;
		U_CHAR a = (0xff000000 & color_value) >> 24;

		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	Color(U_CHAR r, U_CHAR g, U_CHAR b, U_CHAR a) {
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	~Color() {

	}

	Color ToYCbCr() {
		U_CHAR y = static_cast<U_CHAR>(
			std::round(
				.299 * static_cast<float>(this->r) + .587 * static_cast<float>(this->g) + .114 * static_cast<float>(this->b)
			)
		);
		U_CHAR cb = static_cast<U_CHAR>(
			std::round(
				- .169 * static_cast<float>(this->r) - .331 * static_cast<float>(this->g) + .5 * static_cast<float>(this->b) + 128.
			)
		);
		U_CHAR cr = static_cast<U_CHAR>(
			std::round(
				.5 * static_cast<float>(this->r) - .419 * static_cast<float>(this->g) - .081 * static_cast<float>(this->b) + 128.
			)
		);

		return Color(y, cb, cr, 255);
	}

	Color ToRGB() {
		float y = static_cast<float>(this->r);
		float cb = static_cast<float>(this->g) - 128.;
		float cr = static_cast<float>(this->b) - 128.;

		U_CHAR r = static_cast<U_CHAR>(std::round(y + 45. * cr / 32.));
		U_CHAR g = static_cast<U_CHAR>(std::round(y + (11. * cb + 23. * cr) / 32.));
		U_CHAR b = static_cast<U_CHAR>(std::round(y + 113. * cb / 64));

		new Color(r, g, b, this->a);
	}

	void Log() {
		std::cout << "r: " << this->r
			<< " g: " << this->g
			<< " b: " << this->b
			<< " a: " << this->a
			<< std::endl;
	}

	int GetHash() {
		return
			(this->r * 3 + this->g * 5 + this->b * 7 + this->a * 11) % 64;
	}

	int To32BitValue() {
		return (this->r << 0) + (this->g << 8) + (this->b << 16) + (this->a << 24);
	}
};