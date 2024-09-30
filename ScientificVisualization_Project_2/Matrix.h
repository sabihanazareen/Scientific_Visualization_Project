class Matrix {
private:

public:
	int row, col;
	int getIndex(int i, int j);
	double* data;
	Matrix(int row, int col);
	Matrix(const Matrix& M);
	Matrix multiply(Matrix in);
	~Matrix();
	Matrix operator=(const Matrix& in);
};