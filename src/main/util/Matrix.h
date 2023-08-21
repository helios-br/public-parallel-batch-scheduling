#pragma once

#include <iostream>

#include <vector>

using namespace std;

template<typename T>
struct Matrix {

    int rows;
    int cols;
    int maxSize;
    T* data;

public:
    Matrix(int numRows, int numCols) : rows(numRows), cols(numCols) {
    	maxSize = numRows * numCols;
        data = new T[maxSize];
    }

    ~Matrix() {
        delete[] data;
    }

    T& operator()(int row, int col) {
        return data[row * cols + col];
    }

    T* begin() {
        return data;
    }

    T* end() {
        return data + (rows * cols);
    }

    T* getRow(int row) {
    	T* rowData = new T[cols];
    	for (int col = 0; col < cols; col++) {
    		rowData[col] = data[row * cols + col];
    	}
    }
};
