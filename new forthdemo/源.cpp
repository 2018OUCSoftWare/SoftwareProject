#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")


#define BOXFILTER      1
#define MOTINEFILTER   2
#define EDGEFILTER     3
#define SHARPENFILTER  4
#define EMBOSSFILTER   5
#define GAUSSFILTER    6

int imFilter(char* pathIn, char* pathOut, int flag);
// The first conv filter
int boxFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen);
// The second conv filter
int motionFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen);
// The third conv filter
int edgeFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen);
// The fourth conv filter
int sharpenFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen);
// The fifth conv filter
int embossFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen);
// The sixth conv filter
int gaussFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen);


int main()
{
	char* srcPath = "lena.jpg";

	GDALAllRegister();

	// the first conv filter
	imFilter(srcPath, "01_boxfilter.tif", BOXFILTER);
	// the second conv filter
	imFilter(srcPath, "02_motionfilter.tif", MOTINEFILTER);
	// the third conv filter
	imFilter(srcPath, "03_edgefilter.tif", EDGEFILTER);
	// the fourth conv filter
	imFilter(srcPath, "04_sharpenfilter.tif", SHARPENFILTER);
	// the fifth conv filter
	imFilter(srcPath, "05_embossfilter.tif", EMBOSSFILTER);
	// the sixth conv filter
	imFilter(srcPath, "06_gaussfilter.tif", GAUSSFILTER);

	return 0;

}


int imFilter(char* srcPath, char* dstPath, int flag)
{
	// the input dataset
	GDALDataset* poSrcDS;
	// the output dataset
	GDALDataset* poDstDS;
	int imgXlen, imgYlen, bandNum;
	int i;
	float* imgIn;
	float* imgOut;

	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);

	imgIn = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));
	imgOut = (float*)CPLMalloc(imgXlen * imgYlen * sizeof(float));


	for (i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, imgIn, imgXlen, imgYlen, GDT_Float32, 0, 0);
		if (flag == BOXFILTER)
		{
			boxFilter(imgIn, imgOut, imgXlen, imgYlen);
		}
		else if (flag == MOTINEFILTER)
		{
			motionFilter(imgIn, imgOut, imgXlen, imgYlen);
		}
		else if (flag == EDGEFILTER)
		{
			edgeFilter(imgIn, imgOut, imgXlen, imgYlen);
		}
		else if (flag == SHARPENFILTER)
		{
			sharpenFilter(imgIn, imgOut, imgXlen, imgYlen);
		}
		else if (flag == EMBOSSFILTER)
		{
			embossFilter(imgIn, imgOut, imgXlen, imgYlen);
		}
		else if (flag = GAUSSFILTER)
		{
			embossFilter(imgIn, imgOut, imgXlen, imgYlen);
		}

		poDstDS->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, imgOut, imgXlen, imgYlen, GDT_Float32, 0, 0);
	}


	CPLFree(imgIn);
	CPLFree(imgOut);
	GDALClose(poDstDS);
	GDALClose(poSrcDS);

	return 0;
}


// the first conv filter
int boxFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen)
{
	// | 0  1  0 |
	// | 1  1  1 | * 0.2
	// | 0  1  0 |
	int i, j;
	for (j = 1; j<imgYlen - 1; j++)
	{
		for (i = 1; i<imgXlen - 1; i++)
		{
			imgOut[j*imgXlen + i] = (imgIn[(j - 1)*imgXlen + i] +
				imgIn[j*imgXlen + i - 1] +
				imgIn[j*imgXlen + i] +
				imgIn[j*imgXlen + i + 1] +
				imgIn[(j + 1)*imgXlen + i]) / 5.0f;
		}

	}
	return 0;
}

// the second conv filter
int motionFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen)
{
	// | 1  0  0  0  0 |
	// | 0  1  0  0  0 |
	// | 0  0  1  0  0 | * 0.2
	// | 0  0  0  1  0 |
	// | 0  0  0  0  1 |
	int i, j;
	for (j = 2; j<imgYlen - 2; j++)
	{
		for (i = 2; i<imgXlen - 2; i++)
		{
			imgOut[j*imgXlen + i] = (imgIn[(j - 2)*imgXlen + i - 2] +
				imgIn[(j - 1)*imgXlen + i - 1] +
				imgIn[j*imgXlen + i] +
				imgIn[(j + 1)*imgXlen + i + 1] +
				imgIn[(j + 2)*imgXlen + i + 2]) / 5.0f;
		}
	}
	return 0;
}

// the third conv filter
int edgeFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen)
{
	// | -1 -1 -1 |
	// | -1  8 -1 |
	// | -1 -1 -1 |
	int i, j;
	for (j = 1; j<imgYlen - 1; j++)
	{
		for (i = 1; i<imgXlen - 1; i++)
		{
			imgOut[j*imgXlen + i] = (-imgIn[(j - 1)*imgXlen + i - 1]
				- imgIn[(j - 1)*imgXlen + i]
				- imgIn[(j - 1)*imgXlen + i + 1]
				- imgIn[j*imgXlen + i - 1]
				+ imgIn[j*imgXlen + i] * 8
				- imgIn[j*imgXlen + i + 1]
				- imgIn[(j + 1)*imgXlen + i - 1]
				- imgIn[(j + 1)*imgXlen + i]
				- imgIn[(j + 1)*imgXlen + i + 1]);
		}
	}
	return 0;
}

// the fourth conv filter
int sharpenFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen)
{
	// | -1 -1 -1 |
	// | -1  9 -1 |
	// | -1 -1 -1 |
	int i, j;
	for (j = 1; j<imgYlen - 1; j++)
	{
		for (i = 1; i<imgXlen - 1; i++)
		{
			imgOut[j*imgXlen + i] = (-imgIn[(j - 1)*imgXlen + i - 1]
				- imgIn[(j - 1)*imgXlen + i]
				- imgIn[(j - 1)*imgXlen + i + 1]
				- imgIn[j*imgXlen + i - 1]
				+ imgIn[j*imgXlen + i] * 9
				- imgIn[j*imgXlen + i + 1]
				- imgIn[(j + 1)*imgXlen + i - 1]
				- imgIn[(j + 1)*imgXlen + i]
				- imgIn[(j + 1)*imgXlen + i + 1]);
		}
	}
	return 0;
}

// the fifth conv filter
int embossFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen)
{
	// | -1 -1  0 |
	// | -1  0  1 |
	// |  0  1  1 |
	int i, j;
	for (j = 1; j<imgYlen - 1; j++)
	{
		for (i = 1; i<imgXlen - 1; i++)
		{
			imgOut[j*imgXlen + i] = (-imgIn[(j - 1)*imgXlen + i - 1]
				- imgIn[(j - 1)*imgXlen + i]
				- imgIn[j*imgXlen + i - 1]
				+ imgIn[j*imgXlen + i + 1]
				+ imgIn[(j + 1)*imgXlen + i]
				+ imgIn[(j + 1)*imgXlen + i + 1]);
		}
	}
	return 0;
}

// the sixth conv filter
int gaussFilter(float* imgIn, float* imgOut, int imgXlen, int imgYlen)
{
	// | 0.0120  0.1253  0.2736  0.1253  0.0120 |
	// | 0.1253  1.3054  2.8514  1.3054  0.1253 |
	// | 0.2736  2.8514  6.2279  2.8514  0.2736 |
	// | 0.1253  1.3054  2.8514  1.3054  0.1253 |
	// | 0.0120  0.1253  0.2736  0.1253  0.0120 |

	int i, j;
	for (j = 2; j<imgYlen - 2; j++)
	{
		for (i = 2; i<imgXlen - 2; i++)
		{
			imgOut[j*imgXlen + i] = (0.0120*imgIn[(j - 2)*imgXlen + i - 2] +
				0.1253*imgIn[(j - 2)*imgXlen + i - 1] +
				0.2736*imgIn[(j - 2)*imgXlen + i] +
				0.1253*imgIn[(j - 2)*imgXlen + i + 1] +
				0.0120*imgIn[(j - 2)*imgXlen + i + 2] +
				0.1253*imgIn[(j - 1)*imgXlen + i - 2] +
				1.3054*imgIn[(j - 1)*imgXlen + i - 1] +
				2.8514*imgIn[(j - 1)*imgXlen + i] +
				1.3054*imgIn[(j - 1)*imgXlen + i + 1] +
				0.1253*imgIn[(j - 1)*imgXlen + i + 2] +
				0.2763*imgIn[j*imgXlen + i - 2] +
				2.8514*imgIn[j*imgXlen + i - 1] +
				6.2279*imgIn[j*imgXlen + i] +
				2.8514*imgIn[j*imgXlen + i + 1] +
				0.2763*imgIn[j*imgXlen + i + 2] +
				0.1253*imgIn[(j + 1)*imgXlen + i - 2] +
				1.3054*imgIn[(j + 1)*imgXlen + i - 1] +
				2.8514*imgIn[(j + 1)*imgXlen + i] +
				1.3054*imgIn[(j + 1)*imgXlen + i + 1] +
				0.1253*imgIn[(j + 1)*imgXlen + i + 2] +
				0.0120*imgIn[(j + 2)*imgXlen + i - 2] +
				0.1253*imgIn[(j + 2)*imgXlen + i - 1] +
				0.2736*imgIn[(j + 2)*imgXlen + i] +
				0.1253*imgIn[(j + 2)*imgXlen + i + 1] +
				0.0120*imgIn[(j + 2)*imgXlen + i + 2]) / 25.0f;
		}
	}
	return 0;
}

