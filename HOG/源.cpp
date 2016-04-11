#include <opencv/cv.h>;
#include <opencv/highgui.h>;
#include <stdio.h>
#include <ctype.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <io.h>
#include <process.h>
#include <algorithm>
#include <stdlib.h>
#include <vector>
#include <string>
using namespace std;
using namespace cv;

struct k   
{
	string name;
	double result;

	bool operator < (const k &c) const
	{
		return result < c.result;
	}
};
typedef k picture;

#define PI 3.14
int top;
int cmp(picture a,picture b);
void remove(string xsrc,string xdes,string rec,picture x);
void insert(vector<picture> &data,int p,int top,picture temp,int flag);
CvHistogram *calchog(string address);

int main(void)
{
	vector<picture> data0,data1,data2,data3;
	string add,des,sample;
	string des0,des1,des2,des3;
	string arrayrec0,arrayrec1,arrayrec2,arrayrec3;

	cout<<"How many pictures do you wanna save?"<<endl;
	cin>>top;

	cout<<"Target:"<<endl;
	cin>>add;//Ŀ��ͼƬ

	cout<<"Sample:"<<endl;
	cin>>sample;//����

	cout<<"Destination:"<<endl;//��ŵ�ַ
	cin>>des0;
	cin>>des1;
	cin>>des2;
	cin>>des3;

	cout<<"Your result record:"<<endl;
	cin>>arrayrec0;
	cin>>arrayrec1;
	cin>>arrayrec2;
	cin>>arrayrec3;

	CvHistogram *tarhist=0;
	tarhist=calchog(add);

	cout<<"Search..."<<endl;
	string s=sample+"*.jpg";

		//��������
	long handle;                                              
	struct _finddata_t fileinfo;                          
	handle=_findfirst(s.c_str(),&fileinfo);      
	if(-1== handle)
		return -1;

	int p=0;
	cout<<"Running..."<<endl;
	do 
	{
		string temppic=sample+fileinfo.name;

		CvHistogram *temp_hist=0;
		temp_hist=calchog(temppic);

		for(int i=0;i<4;i++)
		{
			int method=i;

			double base_test = cvCompareHist(tarhist,temp_hist,method );
			picture temp;
			temp.name=fileinfo.name;
			temp.result=base_test;

			switch(method)
			{
				case 0:insert(data0,p,top,temp,1);break;
				case 1:insert(data1,p,top,temp,0);break;
				case 2:insert(data2,p,top,temp,1);break;
				case 3:insert(data3,p,top,temp,0);break;
			}
		}
		p++;

		cvReleaseHist(&temp_hist);
	} while (!_findnext(handle,&fileinfo));
	_findclose(handle); 

	for(int i=0;i<top;i++)
	{
		remove(sample,des0,arrayrec0,data0[i]);
		remove(sample,des1,arrayrec1,data1[i]);
		remove(sample,des2,arrayrec2,data2[i]);
		remove(sample,des3,arrayrec3,data3[i]);
	}
	cout<<"Done"<<endl;

	cvReleaseHist(&tarhist);

	return 0;
}

CvHistogram *calchog(string address)
{
	int hdims = 8;// ����HIST�ĸ�����Խ��Խ��ȷ
	float hranges_arr[2] = {-PI/2,PI/2}; // ֱ��ͼ���Ͻ���½�
	float* hranges = hranges_arr;

	IplImage *src = 0;  // source imagre
	IplImage *histimg = 0; // histogram image
	CvHistogram *hist = 0; // define multi_demention histogram
	IplImage* canny;
	CvMat* canny_m;
	IplImage* dx; // the sobel x difference
	IplImage* dy; // the sobel y difference
	CvMat* gradient; // value of gradient
	CvMat* gradient_dir; // direction of gradient
	CvMat* dx_m; // format transform to matrix
	CvMat* dy_m;
	CvMat* mask;
	CvSize  size;
	IplImage* gradient_im;
	int i,j;
	float theta;

	float max_val;
	int bin_w;

	src=cvLoadImage(address.c_str(),0); // force to gray image
	if(src==0) 
	{
		printf("Open Error!\n");
		exit(0);
	}

	size=cvGetSize(src);
	canny=cvCreateImage(cvGetSize(src),8,1);//��Եͼ��
	dx=cvCreateImage(cvGetSize(src),32,1);//x�����ϵĲ��//�˴�����������Ϊ8U ���������
	dy=cvCreateImage(cvGetSize(src),32,1);
	gradient_im=cvCreateImage(cvGetSize(src),32,1);//�ݶ�ͼ��

	canny_m=cvCreateMat(size.height,size.width,CV_32FC1);//��Ե����
	dx_m=cvCreateMat(size.height,size.width,CV_32FC1);
	dy_m=cvCreateMat(size.height,size.width,CV_32FC1);
	gradient=cvCreateMat(size.height,size.width,CV_32FC1);//�ݶȾ���
	gradient_dir=cvCreateMat(size.height,size.width,CV_32FC1);//�ݶȷ������
	mask=cvCreateMat(size.height,size.width,CV_32FC1);//����

	cvCanny(src,canny,60,180,3);//��Ե���
	cvConvert(canny,canny_m);//��ͼ��ת��Ϊ����
	cvSobel(src,dx,1,0,3);// һ��X�����ͼ���� :dx
	cvSobel(src,dy,0,1,3);// һ��Y�����ͼ���� :dy
	cvConvert(dx,dx_m);
	cvConvert(dy,dy_m);

	cvAdd(dx_m,dy_m,gradient); // value of gradient//�ݶȲ��ǵ��ڸ�����x�ĵ�����ƽ������y������ƽ����
	cvDiv(dx_m,dy_m,gradient_dir); // direction

	for(i=0;i<size.height;i++)
		for(j=0;j<size.width;j++)
		{
			if(cvmGet(canny_m,i,j)!=0 && cvmGet(dx_m,i,j)!=0)//������ʲô��˼��ֻ����Ե�ϵķ���
			{
				theta=cvmGet(gradient_dir,i,j);
				theta=atan(theta);
				cvmSet(gradient_dir,i,j,theta);  
			}
			else
			{
				cvmSet(gradient_dir,i,j,0);
			}

		}

	hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );  // ����һ��ָ���ߴ��ֱ��ͼ�������ش�����ֱ��ͼָ��
	histimg = cvCreateImage( cvSize(320,200), 8, 3 ); // ����һ��ͼ��3ͨ��
	cvZero( histimg ); // ��0��
	cvConvert(gradient_dir,gradient_im);//���ݶȷ������ת��Ϊͼ��
	cvCalcHist( &gradient_im, hist, 0, canny ); // ����ֱ��ͼ
	cvNormalizeHist(hist,1);

	cvReleaseImage( &src );
	cvReleaseImage( &histimg );
	return hist;
}

int cmp(picture a,picture b)
{
	return a.result>b.result;
}

void insert(vector<picture> &data,int p,int top,picture temp,int flag)
{
	if(p<top)
		data.push_back(temp);
	else if(p==top)
	{
		data.push_back(temp);
		if(flag)
			std::sort(data.begin(),data.end(),cmp);
		else
			std::sort(data.begin(),data.end());
	}
	else
	{
		data[top]=temp;
		if(flag)
			std::sort(data.begin(),data.end(),cmp);
		else
			std::sort(data.begin(),data.end());
	}
}

void remove(string xsrc,string xdes,string rec,picture x)
{
	string src=xsrc+x.name;
	string des=xdes+x.name;

	int c;
	FILE *fpSrc, *fpDest,*fpRec;  
	fpSrc = fopen(src.c_str(), "rb");   
	if(fpSrc==NULL)
	{
		printf( "Source file open failure!\n");  
		return;
	}

	fpDest = fopen(des.c_str(), "wb");  
	if(fpDest==NULL)
	{
		printf("Destination file open failure!\n");
		return;
	}

	fpRec=fopen(rec.c_str(),"a");

	fprintf(fpRec,"%s %lf\n",x.name.c_str(),x.result);
	while((c=fgetc(fpSrc))!=EOF)
	{   
		fputc(c, fpDest);
	}

	fclose(fpSrc);  
	fclose(fpDest);
	fclose(fpRec);
}