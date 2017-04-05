/*
 * IFVRStereoCamera.cpp
 *
 *  Created on: 2017年2月8日
 *      Author: clover
 */

#include "IFVRStereoCamera.h"
#include <opencv2/opencv.hpp>

IFVRStereoCamera::IFVRStereoCamera(std::string devn, int w, int h, CameraType camType) {
	// TODO Auto-generated constructor stub
	m_cameraType = camType;
	if(m_cameraType == StereoCamera){
		camera = new video_device(devn,w,h);
		m_image_w = w;
		m_image_h = h;
		rgb = new uint8_t[w*h*3];
		gray_L = new uint8_t[w*h];
		gray_R = new uint8_t[w*h];
	}else if(m_cameraType == MonoCamera){
		m_opencvCap = cv::VideoCapture(0);
	}else if(m_cameraType == StereoFile){

	}
}

IFVRStereoCamera::IFVRStereoCamera(std::string Lpath, std::string Rpath, CameraType camType)
{
	m_cameraType = camType;
	m_imageLPath = Lpath;
	m_imageRPath = Rpath;
	m_frameCnt = 0;
}

IFVRStereoCamera::~IFVRStereoCamera() {
	// TODO Auto-generated destructor stub
	delete camera;
	delete [] rgb;
}

void IFVRStereoCamera::update(cv::Mat& lMat, cv::Mat& rMat, bool isFlip, int frameCnt)
{
	if(m_cameraType == StereoCamera){
		if(camera->get_frame(&yuyv, &yuyv_len) == -1)
		{
			std::cout<<"errr"<<std::endl;
		}
		//std::cout<<"-----------------"<<std::endl;
		//std::cout<<(int)yuyv_len<<std::endl;
		//std::cout<<"-----------------"<<std::endl;
		//camera->yuyv_2_rgb888(yuyv, yuyv_len, rgb);
		//camera->yuyv_2_y_gray(yuyv, yuyv_len, gray);
		camera->yuyv_2_lr_gray(yuyv, yuyv_len, gray_L, gray_R);

		cv::Mat mat_l = cv::Mat(cv::Size(m_image_w,m_image_h), CV_8UC1, cv::Scalar::all(0));
		cv::Mat mat_r = cv::Mat(cv::Size(m_image_w,m_image_h), CV_8UC1, cv::Scalar::all(0));
		mat_l.data = gray_L;
		mat_r.data = gray_R;
		if(isFlip){
			cv::flip(mat_l,mat_l,0);
			cv::flip(mat_r,mat_r,0);
		}

		//cv::imshow("L", mat_l);
		lMat = mat_l;

		//cv::imshow("R", mat_r);
		rMat = mat_r;
		camera->unget_frame();
		//int key = cv::waitKey(1);
	}else if(m_cameraType == MonoCamera){
		if(m_opencvCap.isOpened()){
			m_opencvCap >> lMat;
			cv::imshow("mono camera", lMat);
			rMat = lMat;
		}else{
			printf("camera err\n");
			return;
		}
	}else if(m_cameraType == StereoFile){
		sprintf(m_frameName,"%s/image%d.jpg",m_imageLPath.c_str(), frameCnt);
		lMat = cv::imread(m_frameName);
		sprintf(m_frameName,"%s/image%d.jpg",m_imageRPath.c_str(), frameCnt);
		rMat = cv::imread(m_frameName);
		//cv::imshow("L", lMat);
		//cv::imshow("R", rMat);
		//m_frameCnt ++;

	}


}

