#include "Matrix.h"

Matrix::Matrix(int row, int col) {
	data = new double[row * col];
	this->row = row;
	this->col = col;
}

Matrix::~Matrix() {
	delete[] data;
}

int Matrix::getIndex(int i, int j) {
	return i * this->col + j;
}
Matrix Matrix::multiply(Matrix in) {

	Matrix result(this->row, in.col);

	for (int i = 0; i < this->row; i++) {
		for (int j = 0; j < in.col; j++) {
			double sum = 0;

			for (int k = 0; k < this->col; k++) {
				sum = sum + this->data[this->getIndex(i, k)] * in.data[in.getIndex(k, j)];
			}
			result.data[result.getIndex(i, j)] = sum;
		}
	}

	return result;
}

Matrix::Matrix(const Matrix& M) {
	this->row = M.row;
	this->col = M.col;

	this->data = new double[row * col];

	for (int i = 0; i < M.row * M.col; i++) {
		this->data[i] = M.data[i];
	}
}

Matrix Matrix::operator=(const Matrix& M) {
	delete[] this->data;

	this->row = M.row;
	this->col = M.col;

	this->data = new double[row * col];

	for (int i = 0; i < M.row * M.col; i++) {
		this->data[i] = M.data[i];
	}

	return *this;
}