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
	cv::Mat m_mapL1;
	cv::Mat m_mapL2;
	cv::Mat m_mapR1;
	cv::Mat m_mapR2;
	cv::StereoSGBM sgbm;
public:
	CLRStereoCamera(const std::string& e_file, const std::string& i_file, const cv::Size& image_size);
	CLRStereoCamera(int camera_id);
	virtual ~CLRStereoCamera();
	void capture_image(CAPTURE_IMAGE_TYPE cap_type);
	void capture_image_for_calib(CAPTURE_IMAGE_TYPE cap_type, cv::Size chessboard_size);
	void show_stereo_camera_with_calibed(std::string e_file, std::string i_file);
	void capture_video(std::string video_usrname, std::string e_file, std::string i_file);
	void calc_disp_by_calibFiles(
			const cv::Mat& l_src, const cv::Mat& r_src,
			cv::Mat& l_remap, cv::Mat& r_remap, cv::Mat& disp);
};

#endif /* STEREOCAMERA_CLRSTEREOCAMERA_H_ */
