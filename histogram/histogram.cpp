#include <iostream>
#include <fstream>

using namespace std;

void MemFree2D(unsigned char **Mem, int nHeight)
{
	for (int n = 0; n < nHeight; n++)
	{
		delete[] Mem[n];
	}
	delete[] Mem;
}

unsigned char** MemAlloc2D(int nHeight, int nWidth, unsigned char nInitVal)
{
	unsigned char** rtn = new unsigned char*[nHeight];
	for (int n = 0; n < nHeight; n++)
	{
		rtn[n] = new unsigned char[nWidth];
		memset(rtn[n], nInitVal, sizeof(unsigned char) * nWidth);
	}
	return rtn;
}

bool isInsideBoundary(int nHeight, int nWidth, double h, double w)
{
	if (h >= 0 && w >= 0 && h < nHeight && w < nWidth)
	{
		return true;
	}
	else return false;
}

int main()
{
	int width = 256;
	int height = 256;

	int hist[256] = { 0 };
	double histNormal[256] = { 0 };
	int histNormalFinal[256] = { 0 };

	unsigned char **input_img;
	unsigned char **histogram;
	unsigned char **hist_eq;

	FILE *infile;
	fopen_s(&infile, "input.raw", "rb");

	if (infile == NULL)
	{
		cout << "input image error";
		return 0;
	}

	input_img = MemAlloc2D(width, height, 0);
	histogram = MemAlloc2D(width, height, 0);
	hist_eq = MemAlloc2D(width, height, 0);

	for (int h = 0; h < height; h++)
	{
		fread(input_img[h], sizeof(unsigned char), width, infile);
	}

	////////////////hist배열에 각 픽셀 개수 
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			hist[input_img[h][w]]++;
		}
	}

	////////////////누적합 구하기 위해서 배열에 값복사
	for (int i = 0; i < 256; i++)
	{
		histNormal[i] = hist[i];
	}

	//////////////////누적합 구하기
	for (int i = 1; i < 256; i++)
	{
		histNormal[i] = histNormal[i - 1] + histNormal[i];
	}

	////////////////[0,1]의 정규화 후 255 곱하기
	for (int i = 0; i < 256; i++)
	{
		histNormal[i] = histNormal[i] / (width*height) * 255;
	}

	/////////////////[0,1]의 정규화 한것을 int화
	for (int i = 0; i < 256; i++)
	{
		histNormalFinal[i] = histNormal[i];
	}

	////////////////////픽셀수의 비율 구하기 시작
	int nMax = 0;
	for (int i = 0; i < 256; i++)
	{
		if (nMax < hist[i])
			nMax = hist[i];
	}

	double dNormal = 255.0 / nMax;

	/////////////////histogram구하기
	for (int w = 0; w < width; w++)
	{
		int nNormalValue = (int)hist[w] * dNormal;
		for (int h = 0; h < nNormalValue; h++)
		{
			histogram[255 - h][w] = 255;
		}
	}

	//////////////////////////////////////histogram equalization
	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			hist_eq[h][w] = histNormalFinal[input_img[h][w]];
		}
	}

	FILE *outfile;
	FILE *outfileHistEq;
	fopen_s(&outfile, "histogram.raw", "wb");
	fopen_s(&outfileHistEq, "histEq.raw", "wb");

	for (int h = 0; h < 256; h++)
	{
		fwrite(histogram[h], sizeof(unsigned char), 256, outfile);
	}

	for (int h = 0; h < 256; h++)
	{
		fwrite(hist_eq[h], sizeof(unsigned char), 256, outfileHistEq);
	}

	MemFree2D(input_img, 256);
	MemFree2D(histogram, 256);
	MemFree2D(hist_eq, 256);
	fclose(infile);
	fclose(outfile);
	fclose(outfileHistEq);

	return 0;
}