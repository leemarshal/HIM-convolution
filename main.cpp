#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "BMPStruct.h"

using namespace std;

int main()
{
    ifstream templateFile("../assets/hard/apple24_grayscale.bmp", ios::binary);
    ifstream sourceFile("../assets/hard/total24_grayscale.bmp", ios::binary);
    ofstream convMapFile("../assets/hard/convolution.bmp", ios::binary);

    if (!(templateFile && sourceFile)) {
        cerr << "Error opening input.bmp for reading." << endl;
        return 1;
    }

    BMPFILEHEADER templateFileHeader;
    BMPINFOHEADER templateInfoHeader;
    BMPFILEHEADER sourceFileHeader;
    BMPINFOHEADER sourceInfoHeader;
    BMPFILEHEADER convMapFileHeader;
    BMPINFOHEADER convMapInfoHeader;

    templateFile.read(reinterpret_cast<char*>(&templateFileHeader), sizeof(templateFileHeader));
    templateFile.read(reinterpret_cast<char*>(&templateInfoHeader), sizeof(templateInfoHeader));

    sourceFile.read(reinterpret_cast<char*>(&sourceFileHeader), sizeof(sourceFileHeader));
    sourceFile.read(reinterpret_cast<char*>(&sourceInfoHeader), sizeof(sourceInfoHeader));

    convMapFileHeader = sourceFileHeader;
    convMapInfoHeader = sourceInfoHeader;

    convMapFile.write(reinterpret_cast<char*>(&convMapFileHeader), sizeof(convMapFileHeader));
    convMapFile.write(reinterpret_cast<char*>(&convMapInfoHeader), sizeof(convMapInfoHeader));

    RGBTriple black = {0, 0, 0};
    for (int i = 0; i < convMapInfoHeader.height; ++i) {
        for (int j = 0; j < convMapInfoHeader.width; ++j) {
            convMapFile.write(reinterpret_cast<char*>(&black), sizeof(RGBTriple));
        }
    }
    convMapFile.seekp(convMapFileHeader.dataOffset, ios::beg);

    vector<vector<RGBTriple>> templateFlip;
    vector<vector<RGBTriple>> source;
    vector<vector<int>> convolutionMap;
    flipImageHorizontallyAndVertically(templateFile, templateInfoHeader, templateFlip);
    loadBMPToVector(sourceFile, sourceInfoHeader, source);
    convolution(templateFlip, source, convolutionMap, templateInfoHeader, sourceInfoHeader);

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

    templateFile.close();
    sourceFile.close();
    convMapFile.close();

    cout << "convolution complete." << endl;


    return 0;

}
//
// Created by 규북 on 11/1/23.
//
