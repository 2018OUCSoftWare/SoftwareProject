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
	//����ͼ��
	GDALDataset* poSrcDS;
	//���ͼ��
	GDALDataset* poDstDS1;
	GDALDataset* poDstDS2;
	//ͼ������߶�
	int imgXlen, imgYlen;
	//����ͼ��·��
	char* srcPath = "MayFourthSquare.jpg";
	//���ͼ��·��
	char* dstPath1 = "MayFourthSquare1.tif";
	char* dstPath2 = "MayFourthSquare2.tif";
	//ͼ���ڴ�洢
	GByte* buffTmp;
	//ͼ�񲨶���
	int bandNum;
	//��ʼλ������
	int StartX, StartY;
	//�����Ⱥ͸߶�
	int tmpXlen, tmpYlen;

	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath, GA_ReadOnly);
	//��ȡͼ���ȣ��߶ȺͲ�����
	imgXlen = poSrcDS->GetRasterXSize();
	imgYlen = poSrcDS->GetRasterYSize();
	bandNum = poSrcDS->GetRasterCount();

	//����ͼ��Ŀ�Ⱥ͸߶ȷ����ڴ�
	buffTmp = (GByte*)CPLMalloc(imgXlen * imgYlen * sizeof(GByte));
	//�������ͼ��
	poDstDS1 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath1, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	poDstDS2 = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(
		dstPath2, imgXlen, imgYlen, bandNum, GDT_Byte, NULL);
	//һ�������ε����룬Ȼ��һ�������ε����
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
	//ָ��������Ϊ��ɫ
	ChangeColor(300, 300, 100, 50, poSrcDS, poDstDS2, 1, 255);
	ChangeColor(300, 300, 100, 50, poSrcDS, poDstDS2, 2, 255);
	ChangeColor(300, 300, 100, 50, poSrcDS, poDstDS2, 3, 255);
	//ָ��������Ϊ��ɫ
	ChangeColor(500, 500, 50, 100, poSrcDS, poDstDS2, 1, 0);
	ChangeColor(500, 500, 50, 100, poSrcDS, poDstDS2, 2, 0);
	ChangeColor(500, 500, 50, 100, poSrcDS, poDstDS2, 3, 0);
	
	//����ڴ�
	CPLFree(buffTmp);
	//�ر�dataset
	GDALClose(poDstDS1);
	GDALClose(poDstDS2);
	GDALClose(poSrcDS);

	system("PAUSE");
	return 0;
}