//
// Created by 규북 on 11/1/23.
//

#ifndef HIM_CONVOLUTION_BMPSTRUCT_H
#define HIM_CONVOLUTION_BMPSTRUCT_H

#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>

#pragma pack(push, 1) // 정렬을 1바이트 단위로 설정

struct BMPFILEHEADER {
    uint16_t signature;       // 2 bytes
    uint32_t fileSize;        // 4 bytes
    uint16_t reserved1;       // 2 bytes
    uint16_t reserved2;       // 2 bytes
    uint32_t dataOffset;      // 4 bytes
};

struct BMPINFOHEADER {
    uint32_t size;            // 4 bytes
    int32_t width;            // 4 bytes
    int32_t height;           // 4 bytes
    uint16_t planes;          // 2 bytes
    uint16_t bitCount;        // 2 bytes
    uint32_t compression;     // 4 bytes
    uint32_t imageSize;       // 4 bytes
    uint32_t XpixelsPerM;     // 4 bytes
    uint32_t YpixelsPerM;     // 4 bytes
    uint32_t colorsUsed;      // 4 bytes
    uint32_t colorsImportant; // 4 bytes
};

#pragma pack(pop) // 정렬 설정을 원래대로 복원

#pragma pack(push, 1)  // 패딩을 방지하기 위해 사용
struct RGBTriple {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
};
#pragma pack(pop)     // 패딩 설정을 원래대로 돌려놓음

using namespace std;

void convertToGrayscale(ifstream &inFile, ofstream &outFile, BMPINFOHEADER &infoHeader) {
    uint8_t pixel[3];
    uint8_t gray;

    for (int i = 0; i < infoHeader.height; ++i) {
        for (int j = 0; j < infoHeader.width; ++j) {
            inFile.read(reinterpret_cast<char*>(pixel), 3);
            gray = 0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0];
            //gray = 0.333 * pixel[2] + 0.333 * pixel[1] + 0.333 * pixel[0];
            pixel[0] = pixel[1] = pixel[2] = gray;
            outFile.write(reinterpret_cast<char*>(pixel), 3);
        }
    }
}

void flipImageHorizontallyAndVertically(ifstream &inFile, BMPINFOHEADER &infoHeader,
                                        vector<vector<RGBTriple>> &flippedImage) {
    flippedImage.assign(infoHeader.height, vector<RGBTriple>(infoHeader.width));
    RGBTriple pixel;
    int padding = (4 - (static_cast<int>(infoHeader.width * sizeof(RGBTriple)) % 4)) % 4;
    for (int y = 0; y < infoHeader.height; y++) {
        for (int x = 0; x < infoHeader.width; x++) {
            inFile.read(reinterpret_cast<char*>(&pixel), sizeof(RGBTriple));
            flippedImage[infoHeader.height - 1 - y][infoHeader.width - 1 - x] = pixel;
        }
        inFile.ignore(padding);
    }
}

void loadBMPToVector(ifstream &inFile, BMPINFOHEADER &infoHeader,
                     vector<vector<RGBTriple>> &Image) {
    Image.assign(infoHeader.height, vector<RGBTriple>(infoHeader.width));
    RGBTriple pixel;
    int padding = (4 - (static_cast<int>(infoHeader.width * sizeof(RGBTriple)) % 4)) % 4;
    for (int y = 0; y < infoHeader.height; y++) {
        for (int x = 0; x < infoHeader.width; x++) {
            inFile.read(reinterpret_cast<char*>(&pixel), sizeof(RGBTriple));
            Image[y][x] = pixel;
        }
        inFile.ignore(padding);
    }
}

void convolution(vector<vector<RGBTriple>> &small, vector<vector<RGBTriple>> &big,
                 vector<vector<int>> &convolutionMap,
                 BMPINFOHEADER smallInfoHeader, BMPINFOHEADER bigInfoHeader) {
    int h = bigInfoHeader.height - smallInfoHeader.height + 1;
    int w = bigInfoHeader.width - smallInfoHeader.width + 1;
    convolutionMap.assign(h, vector<int>(w));
    int maxValue = 0;
    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            for(int k = 0; k < smallInfoHeader.height; ++k) {
                for(int l = 0; l < smallInfoHeader.width; ++l) {
                    convolutionMap[i][j] += small[k][l].blue * big[i + k][j + l].blue;
                }
            }
            if (convolutionMap[i][j] > maxValue) {
                maxValue = convolutionMap[i][j];
            }
        }
    }
    if(maxValue == 0) return;
    for(int i = 0; i < h; ++i)
        for(int j = 0; j < w; ++j)
            convolutionMap[i][j] = static_cast<int>((static_cast<float>(convolutionMap[i][j]) / maxValue) * 255);
}

//box 치기 grid를 나눠서 local maximum을 찾아보자

#endif //HIM_CONVOLUTION_BMPSTRUCT_H
