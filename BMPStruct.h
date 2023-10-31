//
// Created by 규북 on 11/1/23.
//

#ifndef HIM_CONVOLUTION_BMPSTRUCT_H
#define HIM_CONVOLUTION_BMPSTRUCT_H

#include <cstdint>

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

#endif //HIM_CONVOLUTION_BMPSTRUCT_H
