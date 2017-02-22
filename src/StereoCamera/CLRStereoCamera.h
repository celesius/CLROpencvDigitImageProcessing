/*
 * CLRStereoCamera.h
 *
 *  Created on: 2016年12月1日
 *      Author: clover
 */

#ifndef STEREOCAMERA_CLRSTEREOCAMERA_H_
#define STEREOCAMERA_CLRSTEREOCAMERA_H_

#include <opencv2/opencv.hpp>

enum capture_image_type{
	ATUO_CAPTURE,
	MANUAL_CAPTURE
};

typedef capture_image_type CAPTURE_IMAGE_TYPE;

class CLRStereoCamera {
	void resize_image(const cv::Mat &src, cv::Mat &dist);
	bool get_stereo_chessboard(cv::Mat src, std::vector<cv::Point2f> &l_chessboard, std::vector<cv::Point2f> &r_chessboard);
	void set_frame_size(cv::Size a_size);
	cv::Size m_chessboard_size;
	cv::VideoCapture m_cap;
public:
	CLRStereoCamera(int camera_id);
	virtual ~CLRStereoCamera();
	void capture_image(CAPTURE_IMAGE_TYPE cap_type);
	void capture_image_for_calib(CAPTURE_IMAGE_TYPE cap_type, cv::Size chessboard_size);
	void show_stereo_camera_with_calibed(std::string e_file, std::string i_file);
	void capture_video(std::string video_usrname, std::string e_file, std::string i_file);
};

#endif /* STEREOCAMERA_CLRSTEREOCAMERA_H_ */
