# HIM-convolution

컴파일 실행 안내문

현재 template 이미지는 apple24_grayscale.bmp입니다. 만약 다른 template에 대해서 합성곱을 하고 싶으시면
HIM-convolution/main.cpp에 12번째줄인 string fruit = “apple”;
에 “apple” 대신 “lemon” 또는 “pineapple”을 사용하시면 됩니다.

1. 터미널에서 실행하실 때
소스코드 디렉토리로 이동
<details>
<summary>접기/펼치기</summary>
<div markdown="1">

```

cd (과제 폴더 경로)/20196578/HIM-convolution

```

</div>
</details>

컴파일 (c++ 11이상 버전을 사용하셔야합니다.)
g++ -o convolution main.cpp
실행
./convolution

3. ide에서 실행하실 때
저는 CLion을 작업했으며 main.cpp에서 run하시면 됩니다.

이미지 결과 안내문

1. 과제에 사용한 이미지
1-1. template 이미지
HIM-convolution/assets/template/ 안에 있는
"apple24_grayscale.bmp", "lemon24_grayscale.bmp", "pineapple24_grayscale.bmp"입니다.
1-2. source 이미지
HIM-convolution/assets/source/ 안에 있는 "source24_grayscale.bmp"입니다.

2. 결과
2-1. 결과물 이미지
HIM-convolution/assets/result/ 안에 있는
"apple_redBox.bmp", "lemon_redBox.bmp", "pineapple_redBox.bmp"입니다.
2-2. 합성곱 결과 맵
HIM-convolution/assets/convolutionMap/ 안에 있는
"apple_convolution.bmp", "lemon_convolution.bmp", "pineapple_convolution.bmp"입니다.
2-3. 결과 설명
사과, 레몬, 파인애플 템플릿에 대해서 위치를 잘 찾았습니다. 그러나 대칭성이 약한 바나나, 구스베리에 대해서는 위치를 잘 찾지 못했습니다.
