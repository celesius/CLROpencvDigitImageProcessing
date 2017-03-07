/*
 * IFVRStereoCamera.h
 *
 *  Created on: 2017年2月8日
 *      Author: clover
 */

#ifndef IFVRSTEREOCAMERA_IFVRSTEREOCAMERA_H_
#define IFVRSTEREOCAMERA_IFVRSTEREOCAMERA_H_

#include "video_device.h"
#include <opencv2/opencv.hpp>

enum cameraType{
	MonoCamera,
	StereoCamera
};

typedef cameraType CameraType ;

class IFVRStereoCamera {
	video_device* camera;
	uint8_t* rgb;
	uint8_t* gray_L;
	uint8_t* gray_R;
	uint8_t* yuyv;
	size_t yuyv_len;
	int m_image_w;
	int m_image_h;
	CameraType m_cameraType;
	cv::VideoCapture m_opencvCap;
public:
	IFVRStereoCamera(std::string devn, int w, int h, CameraType camType);
	virtual ~IFVRStereoCamera();
	//void update(uint8_t *image_data);
	void update(cv::Mat& lMat, cv::Mat& rMat);
};

#endif /* IFVRSTEREOCAMERA_IFVRSTEREOCAMERA_H_ */
