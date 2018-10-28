#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

void ChangeColor(int StartX, int StartY, int tmpXlen, int tmpYlen, GDALDataset* poSrcDS, GDALDataset* poDstDS, int ColorKind, int ColorValue)
{
	GByte* buffTmp = (GByte*)CPLMalloc(tmpXlen * tmpYlen * sizeof(GByte));
	poSrcDS->GetRasterBand(ColorKind)->RasterIO(GF_Read,
		StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
	for (int j = 0; j < tmpYlen; j++)
	{
		for (int i = 0; i < tmpXlen; i++)
		{
			buffTmp[j * tmpXlen + i] = (GByte)ColorValue;
		}
	}
	poDstDS->GetRasterBand(ColorKind)->RasterIO(GF_Write,
		StartX, StartY, tmpXlen, tmpYlen, buffTmp, tmpXlen, tmpYlen, GDT_Byte, 0, 0);
}

int main()
{
	//输入图像
	GDALDataset* poSrcDS;
	//输出图像
	GDALDataset* poDstDS1;
	GDALDataset* poDstDS2;
	//图像宽度与高度
	int imgXlen, imgYlen;
	//输入图像路径
	char* srcPath = "MayFourthSquare.jpg";
	//输出图像路径
	char* dstPath1 = "MayFourthSquare1.tif";
	char* dstPath2 = "MayFourthSquare2.tif";
	//图像内存存储
	GByte* buffTmp;
	//图像波段数
	int bandNum;
	//起始位置坐标
	int StartX, StartY;
	//区域宽度和高度
	int tmpXlen, tmpYlen;

	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	//获取图像宽度，高度和波段数
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	//根据图像的宽度和高度分配内存
	buffTmp = (GByte*)CPLMalloc(imgXlen * imgYlen * sizeof(GByte));
	//创建输出图像
	poDstDS1 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath1, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	poDstDS2 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath2, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//一个个波段的输入，然后一个个波段的输出
	for (int i = 0; i < bandNum; i++)
	{
		poSrcDS->GetRasterBand(i + 1)->RasterIO(GF_Read,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS1->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
		poDstDS2->GetRasterBand(i + 1)->RasterIO(GF_Write,
			0, 0, imgXlen, imgYlen, buffTmp, imgXlen, imgYlen, GDT_Byte, 0, 0);
	}

	ChangeColor(100, 100, 200, 150, poSrcDS, poDstDS1, 1, 255);
	//指定区域置为白色
	ChangeColor(300, 300, 100, 50, poSrcDS, poDstDS2, 1, 255);
	ChangeColor(300, 300, 100, 50, poSrcDS, poDstDS2, 2, 255);
	ChangeColor(300, 300, 100, 50, poSrcDS, poDstDS2, 3, 255);
	//指定区域置为黑色
	ChangeColor(500, 500, 50, 100, poSrcDS, poDstDS2, 1, 0);
	ChangeColor(500, 500, 50, 100, poSrcDS, poDstDS2, 2, 0);
	ChangeColor(500, 500, 50, 100, poSrcDS, poDstDS2, 3, 0);
	
	//清除内存
	CPLFree(buffTmp);
	//关闭dataset
	GDALClose(poDstDS1);
	GDALClose(poDstDS2);
	GDALClose(poSrcDS);

	system("PAUSE");
	return 0;
}