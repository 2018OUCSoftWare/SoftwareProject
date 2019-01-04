#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

void IHSImageFusion(float **RGB, float *panBuffTmp, float **IHS, float **resultRGB, int imgXlen, int imgYlen)
{
	/*
	IHSͼ���ںϺ����㷨
	param: RGB-�����ͼ���RGBֵ panBuffTmp-ȫɫͼ�������ֵ IHS-�����ⶨ���IHS
	resultRGB-�洢�ںϺ��ͼ������ֵ imgXlen,imgYlen-ͼ��ĳ��ȺͿ��
	*/
	float matrix[3][3] = { { 1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0 },{ -sqrt(2.0) / 6.0, -sqrt(2.0) / 6.0, sqrt(2.0) / 3.0 },{ 1.0 / sqrt(2.0), -1.0 / sqrt(2.0), 0 } };
	float matrixReverse[3][3] = { { 1.0, -1.0 / sqrt(2.0), 1.0 / sqrt(2.0) },{ 1.0, -1.0 / sqrt(2.0), -1.0 / sqrt(2.0) },{ 1.0, sqrt(2.0), 0 } };
	// RGB2IHS
	for (int m = 0; m < 3; m++)
	{
		for (int n = 0; n < imgXlen*imgYlen; n++)
		{
			IHS[m][n] = 0;	// ��ʼ��Ϊ0��������Ҫ��ε���
			for (int k = 0; k < 3; k++)
			{
				IHS[m][n] += matrix[m][k] * RGB[k][n];
			}
		}
	}
	// ��panͼ�������û�I
	// IHS[0] = panBuffTmp; ָ�븳ֵ�ᵼ��IHS[0]ָ��panBuffTmp����һ�ε��ô˺����ͻḳֵʧ��
	for (int i = 0; i < imgXlen*imgYlen; i++)
	{
		IHS[0][i] = panBuffTmp[i];
	}
	// ��任
	for (int mm = 0; mm < 3; mm++)
	{
		for (int nn = 0; nn < imgXlen*imgYlen; nn++)
		{
			resultRGB[mm][nn] = 0;	// ��ʼ��Ϊ0��������Ҫ��ε���
			for (int kk = 0; kk < 3; kk++)
			{
				resultRGB[mm][nn] += matrixReverse[mm][kk] * IHS[kk][nn];
			}
		}
	}
}

void IHSBigImageFusionByBlock(char *srcMULPath, char *srcPANPath, char * resPath)
{
	/*
	�ֿ�ͼ���ں�
	param: �����ͼ��·����ȫɫͼ��·�����ںϽ��ͼ��·��
	*/
	GDALAllRegister();
	// ��Ĵ�С
	int blockLen = 256;
	GDALDataset* srcMULDS = (GDALDataset*)GDALOpenShared(srcMULPath, GA_ReadOnly);
	GDALDataset* srcPANDS = (GDALDataset*)GDALOpenShared(srcPANPath, GA_ReadOnly);

	int imgMULXlen = srcMULDS->GetRasterXSize();
	int imgMULYlen = srcMULDS->GetRasterYSize();
	int MULBandNum = srcMULDS->GetRasterCount();

	GDALDataset *resDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(resPath, imgMULXlen, imgMULYlen, MULBandNum, GDT_Byte, NULL);

	cout << "MUL image X len: " << imgMULXlen << endl;
	cout << "MUL image Y len: " << imgMULYlen << endl;
	cout << "MUL image Band num : " << MULBandNum << endl;

	int imgPANXlen = srcPANDS->GetRasterXSize();
	int imgPANYlen = srcPANDS->GetRasterYSize();
	int PANBandNum = srcPANDS->GetRasterCount();
	cout << "PAN image X len: " << imgPANXlen << endl;
	cout << "PAN image Y len: " << imgPANYlen << endl;
	cout << "PAN image Band num : " << PANBandNum << endl;

	// ������ұ�ʣ������غ�����ʣ�������
	int blocksX = imgMULXlen / blockLen;
	int blocksY = imgMULYlen / blockLen;
	int restX = imgMULXlen - blocksX * blockLen;
	int restY = imgMULYlen - blocksY * blockLen;
	cout << "Blocks num: X-" << blocksX << "   Y-" << blocksY << endl;
	cout << "Rest pixels: X-" << restX << "   Y-" << restY << endl;

	float *panBuffTmp;
	float **RGB;
	float **resultRGB;
	float **IHS;
	RGB = (float**)CPLMalloc(MULBandNum * sizeof(float));
	resultRGB = (float**)CPLMalloc(MULBandNum * sizeof(float));
	IHS = (float**)CPLMalloc(MULBandNum * sizeof(float));
	panBuffTmp = (float*)CPLMalloc(blockLen*blockLen * sizeof(float));

	for (int k = 0; k < 3; k++)
	{
		RGB[k] = (float*)CPLMalloc(blockLen*blockLen * sizeof(float));
		IHS[k] = (float*)CPLMalloc(blockLen*blockLen * sizeof(float));
		resultRGB[k] = (float*)CPLMalloc(blockLen*blockLen * sizeof(float));
	}
	int X, Y;
	for (int m = 0; m <= blocksY; m++)
	{
		for (int n = 0; n <= blocksX; n++)
		{
			// ������ʵ�ʴ�С
			if (n == blocksX)
			{
				X = restX;
			}
			else if (m == blocksY)
			{
				Y = restY;
			}
			else
			{
				X = Y = blockLen;
			}
			//cout << "Working..." << endl;
			//cout << "Xoff:" << n*blockLen << "    Yoff:" << m*blockLen << endl;
			//cout << "Block size:" << X << "*" << Y << "=" << X*Y << endl;

			// ���ݿ�Ĵ�С����ͼ���ж�ȡ����
			srcPANDS->GetRasterBand(1)->RasterIO(GF_Read, n*blockLen, m*blockLen, X, Y, panBuffTmp, X, Y, GDT_Float32, 0, 0);
			for (int j = 0; j < 3; j++)
			{
				srcMULDS->GetRasterBand(j + 1)->RasterIO(GF_Read, n*blockLen, m*blockLen, X, Y, RGB[j], X, Y, GDT_Float32, 0, 0);
			}

			// �����㷨�ں�
			IHSImageFusion(RGB, panBuffTmp, IHS, resultRGB, X, Y);

			// ����д��ͼ��
			for (int i = 0; i < 3; i++)
			{
				resDS->GetRasterBand(i + 1)->RasterIO(GF_Write, n*blockLen, m*blockLen, X, Y, resultRGB[i], X, Y, GDT_Float32, 0, 0);
			}
		}
	}
	// �ͷ��ڴ�
	CPLFree(panBuffTmp);
	CPLFree(RGB);
	CPLFree(IHS);
	CPLFree(resultRGB);
	GDALClose(srcMULDS);
	GDALClose(srcPANDS);
	GDALClose(resDS);
}
void IHSBigImageFusionByLine(char *srcMULPath, char *srcPANPath, char * resPath)
{
	/*
	����ͼ���ں�
	param: �����ͼ��·����ȫɫͼ��·�����ںϽ��ͼ��·��
	*/
	GDALAllRegister();

	GDALDataset* srcMULDS = (GDALDataset*)GDALOpenShared(srcMULPath, GA_ReadOnly);
	GDALDataset* srcPANDS = (GDALDataset*)GDALOpenShared(srcPANPath, GA_ReadOnly);

	int imgMULXlen = srcMULDS->GetRasterXSize();
	int imgMULYlen = srcMULDS->GetRasterYSize();
	int MULBandNum = srcMULDS->GetRasterCount();
	int lineHeight = 256;		// �и߶�
	int lineLen = imgMULXlen;	// �г���

	GDALDataset *resDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(resPath, imgMULXlen, imgMULYlen, MULBandNum, GDT_Byte, NULL);

	cout << "MUL image X len: " << imgMULXlen << endl;
	cout << "MUL image Y len: " << imgMULYlen << endl;
	cout << "MUL image Band num : " << MULBandNum << endl;

	int imgPANXlen = srcPANDS->GetRasterXSize();
	int imgPANYlen = srcPANDS->GetRasterYSize();
	int PANBandNum = srcPANDS->GetRasterCount();
	cout << "PAN image X len: " << imgPANXlen << endl;
	cout << "PAN image Y len: " << imgPANYlen << endl;
	cout << "PAN image Band num : " << PANBandNum << endl;

	// ����ʣ�������涨�и߶ȵ��еĸ߶�
	int linesY = imgMULYlen / lineHeight;
	int restY = imgMULYlen - lineHeight * linesY;
	cout << "Line num:" << linesY << endl;
	cout << "Rest pixel:" << restY << endl;

	float *panBuffTmp;
	float **RGB;
	float **resultRGB;
	float **IHS;
	RGB = (float**)CPLMalloc(MULBandNum * sizeof(float));
	resultRGB = (float**)CPLMalloc(MULBandNum * sizeof(float));
	IHS = (float**)CPLMalloc(MULBandNum * sizeof(float));
	panBuffTmp = (float*)CPLMalloc(lineHeight*lineLen * sizeof(float));

	for (int k = 0; k < 3; k++)
	{
		RGB[k] = (float*)CPLMalloc(lineHeight*lineLen * sizeof(float));
		IHS[k] = (float*)CPLMalloc(lineHeight*lineLen * sizeof(float));
		resultRGB[k] = (float*)CPLMalloc(lineHeight*lineLen * sizeof(float));
	}
	int Y;
	for (int i = 0; i <= linesY; i++)
	{
		// �����е�ʵ�ʸ߶�
		if (i == linesY)
		{
			Y = restY;
		}
		else
		{
			Y = lineHeight;
		}
		cout << "Working" << endl;
		cout << "Line " << i << "   Xoff:0  Yoff:" << i*lineHeight << endl;
		cout << "Line size:" << Y << "*" << lineLen << "=" << lineLen*Y << endl;
		// ��ȡͼ������
		srcPANDS->GetRasterBand(1)->RasterIO(GF_Read, 0, i*lineHeight, lineLen, Y, panBuffTmp, lineLen, Y, GDT_Float32, 0, 0);
		for (int j = 0; j < 3; j++)
		{
			srcMULDS->GetRasterBand(j + 1)->RasterIO(GF_Read, 0, i*lineHeight, lineLen, Y, RGB[j], lineLen, Y, GDT_Float32, 0, 0);
		}
		// �����㷨����
		IHSImageFusion(RGB, panBuffTmp, IHS, resultRGB, lineLen, Y);
		// ����д��ͼ��
		for (int k = 0; k < 3; k++)
		{
			resDS->GetRasterBand(k + 1)->RasterIO(GF_Write, 0, i*lineHeight, lineLen, Y, resultRGB[k], lineLen, Y, GDT_Float32, 0, 0);
		}
	}
	// �ͷ��ڴ�
	CPLFree(panBuffTmp);
	CPLFree(RGB);
	CPLFree(IHS);
	CPLFree(resultRGB);
	GDALClose(srcMULDS);
	GDALClose(srcPANDS);
	GDALClose(resDS);
}

int main()
{
	char* srcMULPath = "Mul_large.tif";
	char* srcPANPath = "Pan_large.tif";
	char* resPathByBlock = "Byblock.tif";
	char* resPathByLine = "Byline.tif";
	// ��¼ʱ��
	int t1 = clock();
	IHSBigImageFusionByLine(srcMULPath, srcPANPath, resPathByLine);
	int t2 = clock();
	IHSBigImageFusionByBlock(srcMULPath, srcPANPath, resPathByBlock);
	int t3 = clock();
	// ���ʱ��
	cout << "Image fusion by line time:" << (t2 - t1) / CLOCKS_PER_SEC << endl;
	cout << "Image fusion by block time:" << (t3 - t2) / CLOCKS_PER_SEC << endl;
	system("pause");
	return 0;
}