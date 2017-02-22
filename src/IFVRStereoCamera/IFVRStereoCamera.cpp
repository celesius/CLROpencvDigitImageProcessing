/*
 * IFVRStereoCamera.cpp
 *
 *  Created on: 2017年2月8日
 *      Author: clover
 */

#include "IFVRStereoCamera.h"
#include <opencv2/opencv.hpp>
IFVRStereoCamera::IFVRStereoCamera(std::string devn, int w, int h) {
	// TODO Auto-generated constructor stub
	camera = new video_device(devn,w,h);
	m_image_w = w;
	m_image_h = h;
	rgb = new uint8_t[w*h*3];
	gray_L = new uint8_t[w*h];
	gray_R = new uint8_t[w*h];
}

IFVRStereoCamera::~IFVRStereoCamera() {
	// TODO Auto-generated destructor stub
	delete camera;
	delete [] rgb;
}

void IFVRStereoCamera::update(uint8_t* image_data){
	if(camera->get_frame(&yuyv, &yuyv_len) == -1)
	{
		std::cout<<"errr"<<std::endl;
	}
	std::cout<<"-----------------"<<std::endl;
	std::cout<<(int)yuyv_len<<std::endl;
	std::cout<<"-----------------"<<std::endl;
	//camera->yuyv_2_rgb888(yuyv, yuyv_len, rgb);
	//camera->yuyv_2_y_gray(yuyv, yuyv_len, gray);
	camera->yuyv_2_lr_gray(yuyv, yuyv_len, gray_L, gray_R);

	cv::Mat mat_l = cv::Mat(cv::Size(m_image_w,m_image_h), CV_8UC1, cv::Scalar::all(0));
	mat_l.data = gray_L;
	cv::imshow("L", mat_l);

	cv::Mat mat_r = cv::Mat(cv::Size(m_image_w,m_image_h), CV_8UC1, cv::Scalar::all(0));
	mat_r.data = gray_R;
	cv::imshow("R", mat_r);




	camera->unget_frame();

	int key = cv::waitKey(1);

}

