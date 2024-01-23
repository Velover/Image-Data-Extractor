#pragma once
#include <iostream>
#include <tuple>
#include <array>
#include <deque>
#include <algorithm>

const float QUATER = 1. / 4.;
const float INVERSE_SQRT_2 = 1. / std::sqrt(2);
const double PI = 3.14159265358979323846;

static inline float C(int x) {
	return x == 0 ? INVERSE_SQRT_2 : 1;
}

static inline float CosineProduct(const float m, const float n, const float x, const float y) {
	using namespace std;
	return cos((2. * x + 1.) * m * PI / (2. * 8.)) * cos((2. * y + 1.) * n * PI / (2. * 8.));
}

//Matrix 8x8
template <typename DataType = double>
class Matrix {
private:
	std::array<std::array<DataType, 8>, 8> data;
public:

	Matrix() {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				this->data[y][x] = 0;
			}
		}
	}

	Matrix(std::array<DataType, 64> new_data) {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				this->data[y][x] = new_data[y * 8 + x];
			}
		}
	}

	~Matrix() {}

	DataType& At(int x, int y) {
		return this->data[y][x];
	}

	void Multiply(DataType multiplicator) {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				this->data[y][x] *= multiplicator;
			}
		}
	}

	void Offset(DataType offset) {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				this->data[y][x] += offset;
			}
		}
	}

	const float NumberToDCT(const int u, const int v) {
		const float g = QUATER * C(u) * C(v);
		float summ = 0;
		for (int n = 0; n < 8; n++) {
			for (int p = 0; p < 8; p++) {
				const float cosine_product = CosineProduct(u, v, p, n);
				summ += this->At(n, p) * cosine_product;
			}
		}
		return g * summ;
	}

	const float NumberFromDCT(const int x, const int y) {
		float summ = 0;
		for (int m = 0; m < 8; m++) {
			for (int n = 0; n < 8; n++) {
				const float cosine_product = CosineProduct(m, n, x, y);
				summ += this->At(n, m) * C(m) * C(n) * cosine_product;
			}
		}

		return QUATER * summ;
	}

	Matrix<DataType>& MatrixToDCT() {
		Matrix<DataType> dct_matrix;

		for (int v = 0; v < 8; v++) {
			for (int u = 0; u < 8; u++) {
				dct_matrix.At(u, v) = this->NumberToDCT(u, v);;
			}
		}

		return dct_matrix;
	}

	Matrix<DataType>& DCTToMatrix() {
		Matrix<DataType> matrix;
		for (int v = 0; v < 8; v++) {
			for (int u = 0; u < 8; u++) {
				matrix.At(u, v) = this->NumberFromDCT(u, v);
			}
		}

		return matrix;
	}

	void Round() {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				this->data[y][x] = std::round(this->data[y][x]);
			}
		}
	}

	template <typename NewType>
	Matrix<NewType> Convert() {
		Matrix<NewType> converted_matrix;
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				converted_matrix.At(x, y) = static_cast<NewType>(this->data[y][x]);
			}
		}
		return converted_matrix;
	}

	void Quantize(Matrix<DataType>& quantization_table) {
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				this->data[y][x] /= quantization_table.At(x, y);
			}
		}
	}

	std::deque<DataType> ToDiagonalIntArray() {
		std::deque<DataType> diagonal_data;

		for (int line = 1; line < 8 + 8; line++) {
			const int start_collumn = std::max(0, line - 8);
			const int count = std::min(std::min(line, (8 - start_collumn)), 8);
			for (int j = 0; j < count; j++) {
				const int y = std::min(8, line) - j - 1;
				const int x = start_collumn + j;
				DataType value = this->data[x][y];
				diagonal_data.push_back(value);
			}
		}
		return diagonal_data;
	}

	template <typename OutputType = DataType>
	void LogMatrix() {
		std::string text = "";
		for (int y = 0; y < 8; y++) {
			for (int x = 0; x < 8; x++) {
				text += std::to_string(static_cast<OutputType>(data[y][x])) + " ";
			}
			text += "\n";
		}
		Log(text);
	}
};