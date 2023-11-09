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

int convolution(vector<vector<RGBTriple>> &small, vector<vector<RGBTriple>> &big,
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
    if(maxValue == 0) return maxValue;
    for(int i = 0; i < h; ++i)
        for(int j = 0; j < w; ++j)
            convolutionMap[i][j] = static_cast<int>((static_cast<float>(convolutionMap[i][j]) / maxValue) * 255);
    return maxValue;
}

void drawConvMap(BMPINFOHEADER &templateInfoHeader, BMPINFOHEADER &sourceInfoHeader, BMPINFOHEADER &convMapInfoHeader,
                 vector<vector<int>> &convolutionMap, ofstream &convMapFile) {
    RGBTriple temp;
    for(int i = 0; i < templateInfoHeader.height - 1; ++i)
        convMapFile.seekp(convMapInfoHeader.width * 3, ios::cur);
    for (int i = 0; i < sourceInfoHeader.height - templateInfoHeader.height + 1; ++i) {
        convMapFile.seekp(sizeof(RGBTriple) * (templateInfoHeader.width - 1), ios::cur);
        for (int j = 0; j < sourceInfoHeader.width - templateInfoHeader.width + 1; ++j) {
            uint8_t t = static_cast<uint8_t>(convolutionMap[i][j]);
            temp = {t, t, t};
            convMapFile.write(reinterpret_cast<char*>(&temp), sizeof(RGBTriple));
        }
        //convMapFile.seekp(sizeof(RGBTriple) * (templateInfoHeader.width - 1), ios::cur);
    }
}

//box 치기 grid를 나눠서 local maximum을 찾아보자
void findLocalMaximum(vector<vector<pair<int, int>>> &localMaximum, vector<vector<int>> &convolutionMap,
                      BMPINFOHEADER &templateInfoHeader, int selfConvolutionValue, int h, int w) {
    int minDiff = 256;
    vector<vector<int>> localMaxTemp(h, vector<int>(w, 0));
    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            int maxTemp = 0;
            for(int k = 0; k < templateInfoHeader.height; ++k) {
                for(int l = 0; l < templateInfoHeader.width; ++l) {
                    int row = i * templateInfoHeader.height + k;
                    int col = j * templateInfoHeader.width + l;
                    if(convolutionMap[row][col] > maxTemp) {
                        maxTemp = convolutionMap[row][col];
                        localMaximum[i][j] = {row, col};
                        localMaxTemp[i][j] = maxTemp;
                    }
                }
            }
            if(abs(maxTemp - selfConvolutionValue) < minDiff) minDiff = abs(maxTemp - selfConvolutionValue);
        }
    }
    for(int i = 0; i < h; ++i)
        for(int j = 0; j < w; ++j)
            if(abs(localMaxTemp[i][j] - selfConvolutionValue) > minDiff)
                localMaximum[i][j] = {-1, -1};
}

void redBox(vector<vector<RGBTriple>> &source, vector<vector<pair<int, int>>> &localMaximum,
            BMPINFOHEADER &templateInfoHeader) {
    for(auto &lm : localMaximum) {
        for(auto l : lm) {
            if(l.first == -1) continue;
            int y = l.first;
            int x = l.second;
            for (; y > max(-64, l.first - 64); --y)
                source[y + templateInfoHeader.height - 1][x + templateInfoHeader.width - 1] = {0, 0, 255};
            for (; x > max(-64, l.second - 64); --x)
                source[y + templateInfoHeader.height - 1][x + templateInfoHeader.width - 1] = {0, 0, 255};
            for (; y <= l.first; ++y)
                source[y + templateInfoHeader.height - 1][x + templateInfoHeader.width - 1] = {0, 0, 255};
            for (; x <= l.second; ++x)
                source[y + templateInfoHeader.height - 1][x + templateInfoHeader.width - 1] = {0, 0, 255};

        }
    }
}

void drawRedBox(vector<vector<RGBTriple>> &source, ofstream &redBoxFile, int w, int h) {
    int rowSize = w * 3;
    int padding = (4 - (rowSize % 4)) % 4;

    for (int y = 0; y < h; ++y) { // BMP는 아래에서 위로 저장됩니다.
        // 현재 행의 모든 픽셀을 씁니다.
        for (int x = 0; x < w; x++) {
            // RGBTriple 형태로 source에서 픽셀 데이터를 읽어서 outFile에 씁니다.
            RGBTriple &pixel = source[y][x];
            redBoxFile.write(reinterpret_cast<char*>(&pixel), 3);
        }
        // 필요한 만큼 padding을 씁니다.
        for (int p = 0; p < padding; p++) {
            char pad = 0;
            redBoxFile.write(&pad, 1);
        }
    }
}

#endif //HIM_CONVOLUTION_BMPSTRUCT_H
