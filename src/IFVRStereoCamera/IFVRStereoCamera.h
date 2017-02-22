/*
 * IFVRStereoCamera.h
 *
 *  Created on: 2017年2月8日
 *      Author: clover
 */

#ifndef IFVRSTEREOCAMERA_IFVRSTEREOCAMERA_H_
#define IFVRSTEREOCAMERA_IFVRSTEREOCAMERA_H_

#include "video_device.h"

class IFVRStereoCamera {
	video_device* camera;
	uint8_t* rgb;
	uint8_t* gray_L;
	uint8_t* gray_R;
	uint8_t* yuyv;
	size_t yuyv_len;
	int m_image_w;
	int m_image_h;
public:
	IFVRStereoCamera(std::string devn, int w, int h);
	virtual ~IFVRStereoCamera();
	void update(uint8_t *image_data);
};

#endif /* IFVRSTEREOCAMERA_IFVRSTEREOCAMERA_H_ */
