#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main()
{
	//����ͼ��
	GDALDataset* poSrcDS,*poSrcDS2;
	//���ͼ��
	GDALDataset* poDstDS;
	//ͼ��Ŀ�Ⱥ͸߶�
	int imgXlen, imgYlen;
	//����ͼ��·��
	char* srcPath1 = "sky.jpg";
	char* srcPath2 = "robot.jpg";
	//����ͼ��·��
	char* dstPath = "res.tif";
	//ͼ���ڴ洢��
	GByte* buffTmp,*buffTmp1,*buffTmp2,*buffTmp3,*buffTmp4,*buffTmp5,*buffTmp6;
	//ͼ�񲨶���
	int i,j, bandNum;

	//ע������
	GDALAllRegister();

	//��ͼ��
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath1, GA_ReadOnly);
	poSrcDS2 = (GDALDataset*)GDALOpenShared(srcPath2, GA_ReadOnly);

	//��ȡͼ���ȣ��߶ȺͲ�������
	imgXlen = poSrcDS ->GetRasterXSize();
	imgYlen = poSrcDS ->GetRasterYSize();
	bandNum = poSrcDS ->GetRasterCount();

	
	//����ͼ��Ŀ�Ⱥ͸߶ȷ����ڴ�

	buffTmp1 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp2 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp3 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp4 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp5 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp6 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));

	//�������ͼ��
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(dstPath,imgXlen,imgYlen,bandNum,GDT_Byte,NULL);

	
	//��ȡ����ͼƬ����ͨ��
	poSrcDS2->GetRasterBand(1)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp1,imgXlen,imgYlen,GDT_Byte,0,0);
	poSrcDS2->GetRasterBand(2)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp2,imgXlen,imgYlen,GDT_Byte,0,0);
	poSrcDS2->GetRasterBand(3)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp3,imgXlen,imgYlen,GDT_Byte,0,0);
	//��ȡ����ͼƬ����ͨ��
	poSrcDS->GetRasterBand(1)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp4,imgXlen,imgYlen,GDT_Byte,0,0);
	poSrcDS->GetRasterBand(2)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp5,imgXlen,imgYlen,GDT_Byte,0,0);
	poSrcDS->GetRasterBand(3)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp6,imgXlen,imgYlen,GDT_Byte,0,0);
	
	for(i=0;i<imgYlen;i++)
	{
		for(j=0;j<imgXlen;j++)
		{
			if( buffTmp1[i*imgXlen+j]>10 && buffTmp1[i*imgXlen+j]<160 )
			{
				if(buffTmp2[i*imgXlen+j]>100 && buffTmp2[i*imgXlen+j]<220)
				{
					if(buffTmp3[i*imgXlen+j]>10 && buffTmp3[i*imgXlen+j]<110)
					{
						
					}
					else 
						{
							buffTmp4[i*imgXlen+j]=buffTmp1[i*imgXlen+j];
						    buffTmp5[i*imgXlen+j]=buffTmp2[i*imgXlen+j];
						    buffTmp6[i*imgXlen+j]=buffTmp3[i*imgXlen+j];
					}
				}
				else{
					    buffTmp4[i*imgXlen+j]=buffTmp1[i*imgXlen+j];
						buffTmp5[i*imgXlen+j]=buffTmp2[i*imgXlen+j];
						buffTmp6[i*imgXlen+j]=buffTmp3[i*imgXlen+j];
				}
			}
			else {
				        buffTmp4[i*imgXlen+j]=buffTmp1[i*imgXlen+j];
						buffTmp5[i*imgXlen+j]=buffTmp2[i*imgXlen+j];
						buffTmp6[i*imgXlen+j]=buffTmp3[i*imgXlen+j];
			}
		}

	}
	poDstDS->GetRasterBand(1)->RasterIO(GF_Write,0,0,imgXlen,imgYlen,buffTmp4,imgXlen,imgYlen,GDT_Byte,0,0);
	poDstDS->GetRasterBand(2)->RasterIO(GF_Write,0,0,imgXlen,imgYlen,buffTmp5,imgXlen,imgYlen,GDT_Byte,0,0);
	poDstDS->GetRasterBand(3)->RasterIO(GF_Write,0,0,imgXlen,imgYlen,buffTmp6,imgXlen,imgYlen,GDT_Byte,0,0);
	//����ڴ�
	//CPLFree(buffTmp);
	CPLFree(buffTmp1);
	CPLFree(buffTmp2);
	CPLFree(buffTmp3);
	CPLFree(buffTmp4);
	CPLFree(buffTmp5);
	CPLFree(buffTmp6);
	//�ر�dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);
	//GDALClose(poSrcDS2);

	system("PAUSE");
	return 0;
}

