#include <iostream>
#include <fstream>
#include <array>
#include "BMPStruct.h"

using namespace std;

void convertToGrayscale(ifstream &inFile, ofstream &outFile, BMPINFOHEADER &infoHeader) {
    uint8_t pixel[3];
    uint8_t gray;

    for (int i = 0; i < infoHeader.height; ++i) {
        for (int j = 0; j < infoHeader.width; ++j) {
            inFile.read(reinterpret_cast<char*>(pixel), 3);
            gray = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
            pixel[0] = pixel[1] = pixel[2] = gray;
            outFile.write(reinterpret_cast<char*>(pixel), 3);
        }
    }
}

void flipImageHorizontallyAndVertically(ifstream &inFile, ofstream &outFile, BMPINFOHEADER &infoHeader) {
    RGBTriple** imageData = new RGBTriple*[infoHeader.height];
    for (int i = 0; i < infoHeader.height; i++) {
        imageData[i] = new RGBTriple[infoHeader.width];
    }

    // 이미지 데이터 읽기
    for (int y = 0; y < infoHeader.height; y++) {
        for (int x = 0; x < infoHeader.width; x++) {
            inFile.read(reinterpret_cast<char*>(&imageData[y][x]), sizeof(RGBTriple));
        }
    }

    // 상하좌우 반전하여 출력
    for (int y = infoHeader.height - 1; y >= 0; y--) {
        for (int x = infoHeader.width - 1; x >= 0; x--) {
            outFile.write(reinterpret_cast<char*>(&imageData[y][x]), sizeof(RGBTriple));
        }
    }

    for (int i = 0; i < infoHeader.height; i++) {
        delete[] imageData[i];
    }
    delete[] imageData;
}

int main()
{
    ifstream inFile("../apple.bmp", ios::binary);
    ofstream outFile("../apple_flip.bmp", ios::binary);

    if (!inFile) {
        cerr << "Error opening input.bmp for reading." << endl;
        return 1;
    }

    BMPFILEHEADER fileHeader;
    BMPINFOHEADER infoHeader;

    inFile.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    inFile.read(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    outFile.write(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));
    outFile.write(reinterpret_cast<char*>(&infoHeader), sizeof(infoHeader));

    flipImageHorizontallyAndVertically(inFile, outFile, infoHeader);

    inFile.close();
    outFile.close();

    cout << "Flip conversion complete." << endl;
    return 0;

}
//
// Created by 규북 on 11/1/23.
//
