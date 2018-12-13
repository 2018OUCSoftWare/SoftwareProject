#include <iostream>
using namespace std;
#include "./gdal/gdal_priv.h"
#pragma comment(lib, "gdal_i.lib")

int main()
{
	//输入图像
	GDALDataset* poSrcDS,*poSrcDS2;
	//输出图像
	GDALDataset* poDstDS;
	//图像的宽度和高度
	int imgXlen, imgYlen;
	//输入图像路径
	char* srcPath1 = "sky.jpg";
	char* srcPath2 = "robot.jpg";
	//输入图像路径
	char* dstPath = "res.tif";
	//图象内存储存
	GByte* buffTmp,*buffTmp1,*buffTmp2,*buffTmp3,*buffTmp4,*buffTmp5,*buffTmp6;
	//图像波段数
	int i,j, bandNum;

	//注册驱动
	GDALAllRegister();

	//打开图像
	poSrcDS = (GDALDataset*)GDALOpenShared(srcPath1, GA_ReadOnly);
	poSrcDS2 = (GDALDataset*)GDALOpenShared(srcPath2, GA_ReadOnly);

	//获取图像宽度，高度和波段数量
	imgXlen = poSrcDS ->GetRasterXSize();
	imgYlen = poSrcDS ->GetRasterYSize();
	bandNum = poSrcDS ->GetRasterCount();

	
	//根据图像的宽度和高度分配内存

	buffTmp1 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp2 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp3 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp4 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp5 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));
	buffTmp6 = (GByte*)CPLMalloc(3*imgXlen*imgYlen*sizeof(GByte));

	//创建输出图像
	poDstDS = GetGDALDriverManager()->GetDriverByName("GTiff")->Create(dstPath,imgXlen,imgYlen,bandNum,GDT_Byte,NULL);

	
	//读取超人图片各个通道
	poSrcDS2->GetRasterBand(1)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp1,imgXlen,imgYlen,GDT_Byte,0,0);
	poSrcDS2->GetRasterBand(2)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp2,imgXlen,imgYlen,GDT_Byte,0,0);
	poSrcDS2->GetRasterBand(3)->RasterIO(GF_Read,0,0,imgXlen,imgYlen,buffTmp3,imgXlen,imgYlen,GDT_Byte,0,0);
	//读取宇宙图片各个通道
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
	//清除内存
	//CPLFree(buffTmp);
	CPLFree(buffTmp1);
	CPLFree(buffTmp2);
	CPLFree(buffTmp3);
	CPLFree(buffTmp4);
	CPLFree(buffTmp5);
	CPLFree(buffTmp6);
	//关闭dataset
	GDALClose(poDstDS);
	GDALClose(poSrcDS);
	//GDALClose(poSrcDS2);

	system("PAUSE");
	return 0;
}

