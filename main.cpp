#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "BMPStruct.h"

using namespace std;

int main()
{

    string fruit = "apple";
    ifstream templateFile("./assets/hard/" + fruit + "24_grayscale.bmp", ios::binary);
    ifstream sourceFile("./assets/hard/source24_grayscale.bmp", ios::binary);
    ofstream convMapFile("./assets/hard/" + fruit + "_convolution.bmp", ios::binary);
    ofstream redBoxFile("./assets/hard/" + fruit + "_redBox.bmp", ios::binary);

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
    BMPFILEHEADER redBoxFileHeader;
    BMPINFOHEADER redBoxInfoHeader;

    templateFile.read(reinterpret_cast<char*>(&templateFileHeader), sizeof(templateFileHeader));
    templateFile.read(reinterpret_cast<char*>(&templateInfoHeader), sizeof(templateInfoHeader));

    sourceFile.read(reinterpret_cast<char*>(&sourceFileHeader), sizeof(sourceFileHeader));
    sourceFile.read(reinterpret_cast<char*>(&sourceInfoHeader), sizeof(sourceInfoHeader));

    convMapFileHeader = sourceFileHeader;
    convMapInfoHeader = sourceInfoHeader;

    redBoxFileHeader = sourceFileHeader;
    redBoxInfoHeader = sourceInfoHeader;

    convMapFile.write(reinterpret_cast<char*>(&convMapFileHeader), sizeof(convMapFileHeader));
    convMapFile.write(reinterpret_cast<char*>(&convMapInfoHeader), sizeof(convMapInfoHeader));

    redBoxFile.write(reinterpret_cast<char*>(&redBoxFileHeader), sizeof(redBoxFileHeader));
    redBoxFile.write(reinterpret_cast<char*>(&redBoxInfoHeader), sizeof(redBoxInfoHeader));

    RGBTriple black = {0, 0, 0};
    for (int i = 0; i < convMapInfoHeader.height; ++i) {
        for (int j = 0; j < convMapInfoHeader.width; ++j) {
            convMapFile.write(reinterpret_cast<char*>(&black), sizeof(RGBTriple));
        }
    }
    convMapFile.seekp(convMapFileHeader.dataOffset, ios::beg);

    vector<vector<RGBTriple>> templateOrigin;

    vector<vector<RGBTriple>> templateFlip;
    vector<vector<RGBTriple>> source;
    vector<vector<int>> convolutionMap;
    vector<vector<int>> convolutionSelf;

    flipImageHorizontallyAndVertically(templateFile, templateInfoHeader, templateFlip);
    templateFile.seekg(templateFileHeader.dataOffset, ios::beg);
    loadBMPToVector(templateFile, templateInfoHeader, templateOrigin);
    loadBMPToVector(sourceFile, sourceInfoHeader, source);

    int convolutionMax = convolution(templateFlip, source, convolutionMap, templateInfoHeader, sourceInfoHeader);
    drawConvMap(templateInfoHeader, sourceInfoHeader, convMapInfoHeader, convolutionMap, convMapFile);

    int self = convolution(templateFlip, templateOrigin, convolutionSelf, templateInfoHeader, templateInfoHeader);
    int selfConvolutionValue = static_cast<int>((static_cast<float>(self) /convolutionMax) * 255);

    int h = (sourceInfoHeader.height - templateInfoHeader.height) / templateInfoHeader.height;
    int w = (sourceInfoHeader.width - templateInfoHeader.width) / templateInfoHeader.width;
    vector<vector<pair<int, int>>> localMaximum(h, vector<pair<int, int>>(w, {-1, -1}));
    findLocalMaximum(localMaximum, convolutionMap, templateInfoHeader, selfConvolutionValue, h, w);

    redBox(source, localMaximum, templateInfoHeader);
    drawRedBox(source,redBoxFile, redBoxInfoHeader.width, redBoxInfoHeader.height);

    templateFile.close();
    sourceFile.close();
    convMapFile.close();
    redBoxFile.close();

    cout << "convolution complete." << endl;


    return 0;

}
//
// Created by 규북 on 11/1/23.
//
