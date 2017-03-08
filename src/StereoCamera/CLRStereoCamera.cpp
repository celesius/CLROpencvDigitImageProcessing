/*
 * CLRStereoCamera.cpp
 *
 *  Created on: 2016年12月1日
 *      Author: clover
 */

#include "CLRStereoCamera.h"
#include "../CLRProcessingCommon.h"
#include <stdio.h>
#include "../CLRCVHelper/CLRCVHelper.hpp"

using namespace std;

CLRStereoCamera::CLRStereoCamera(const std::string& e_file, const std::string& i_file, const cv::Size& image_size){
	bool is_ok = get_remap_mat(i_file, e_file, image_size, m_mapL1, m_mapL2, m_mapR1, m_mapR2);
	sgbm =
    //cv::StereoSGBM::create(   0,   //mindisp
    cv::StereoSGBM(   0,   //mindisp
                   48,   //numdisp
                   5,   //SADWindow
                   600,   //P1
                   2400,   //P2
                   1,   //dispdiffmax
                   63,   //prefiltercap
                   10,   //uniqueness
                   200,   //speckle window size
                   2   //speckle range
                   );

}

CLRStereoCamera::CLRStereoCamera(int camera_id) {
	// TODO Auto-generated constructor stub
	m_cap = cv::VideoCapture(camera_id);
}

CLRStereoCamera::~CLRStereoCamera() {
	// TODO Auto-generated destructor stub
}

void CLRStereoCamera::resize_image(const cv::Mat &src, cv::Mat &dist)
{
	int rows = src.rows;
	int cols = src.cols;
	cv::resize(src, dist, cv::Size(cols/2, rows/2));
}

bool CLRStereoCamera::get_stereo_chessboard(cv::Mat src, std::vector<cv::Point2f> &l_chessboard_point, std::vector<cv::Point2f> &r_chessboard_point)
{
	bool get_l = false;
	bool get_r = false;
	cv::Mat l = splitMat(src, true);
	cv::Mat r = splitMat(src, false);
	//cv::flip(r,r,-1);
	l_chessboard_point.clear();
	r_chessboard_point.clear();

	bool lf = cv::findChessboardCorners(l, m_chessboard_size, l_chessboard_point);
	bool rf = cv::findChessboardCorners(r, m_chessboard_size, r_chessboard_point);

	cv::drawChessboardCorners(l,m_chessboard_size,l_chessboard_point, true);
	cv::drawChessboardCorners(r,m_chessboard_size,r_chessboard_point, true);

	return lf & rf;
}


void CLRStereoCamera::set_frame_size(cv::Size a_size)
{
	m_cap.set(3, 640*2);
	m_cap.set(4, 480);
}


void CLRStereoCamera::capture_image(CAPTURE_IMAGE_TYPE cap_type)
{
	cv::Mat frame;
	cv::Mat small;
//	cv::findChessboardCorners()
	int save_cnt = 0;
	char image_name[50];
	if(m_cap.isOpened()){
		set_frame_size(cv::Size(640,480));
		for(;;){
			m_cap >> frame;
			//resize_image(frame, small);
			cv::imshow("video",frame);
			int key = cv::waitKey(1);
			if(key == 'q'){
				break;
			}else if(key == 's'){
				sprintf(image_name, "img%04d.jpg", save_cnt++);
				cv::imwrite(image_name, frame);
			}
		}
	}else{
		cerr<<"open camera err"<<endl;
	}
}

void CLRStereoCamera::capture_image_for_calib(CAPTURE_IMAGE_TYPE cap_type, cv::Size chessboard_size)
{
	cv::Mat frame;
	cv::Mat frame_wiil_save;
	cv::Mat small;
	m_chessboard_size = chessboard_size;
	vector<cv::Point2f> l_chessboard_point;
	vector<cv::Point2f> r_chessboard_point;
	//	cv::findChessboardCorners()
	int save_cnt = 0;
	char image_name[50];
	//m_cap.set(3, 640*2);
	//m_cap.set(4, 480);
	if(m_cap.isOpened()){
		for(;;){
			m_cap >> frame;
			frame_wiil_save = frame.clone();
			std::string name = std::string(image_name);
			cv::putText(frame,name,cv::Point(50,50),1,1,cv::Scalar(255,100,0));
			//resize_image(frame, small);
			cv::imshow("video",frame);
			int key = cv::waitKey(1);
			if(key == 'q'){
				break;
			}else if(key == 's'){
				if(get_stereo_chessboard(frame, l_chessboard_point, r_chessboard_point))
				{
					sprintf(image_name, "img%04d.jpg", save_cnt++);
					cv::imwrite(image_name, frame_wiil_save);
				}
			}
		}
	}else{
		cerr<<"open camera err"<<endl;
	}
}

void CLRStereoCamera::show_stereo_camera_with_calibed(std::string e_file, std::string i_file)
{
	cv::Mat mapL1;
	cv::Mat mapL2;
	cv::Mat mapR1;
	cv::Mat mapR2;
	bool is_ok = get_remap_mat(i_file, e_file, cv::Size(640,480), mapL1, mapL2, mapR1, mapR2);
	cv::Mat frame;
	cv::Mat remap_frame;
	cv::Mat l;
	cv::Mat r;
	m_cap.set(3, 640*2);
	m_cap.set(4, 480);
	char image_name[30] = {0};
	int save_cnt = 0;

	if(m_cap.isOpened() && is_ok){
		for(;;){
			m_cap >> frame;
			l = splitMat(frame, true);
			r = splitMat(frame, false);
			cv::imshow("ori frame", frame);
			remap( l,  l, mapL1, mapL2, cv::INTER_LINEAR );
			remap( r, r, mapR1, mapR2, cv::INTER_LINEAR );
			remap_frame = combineTwoImg(l, r);
			cv::imshow("remap frame", remap_frame);
			int key = cv::waitKey(1);
			if(key == 'q')
				break;
			else if(key == 's'){
				sprintf(image_name, "./remap_image/img%04d.jpg", save_cnt++);
				std::cout<<cv::imwrite(image_name,remap_frame)<<std::endl;
			}
		}
	}
}

void CLRStereoCamera::capture_video(std::string video_usrname, std::string e_file, std::string i_file){
	//cv::VideoCapture video_capture = cv::VideoCapture()

    //cv::VideoWriter write = cv::VideoWriter("recode.avi",cv::VideoWriter::fourcc('M','J','P','G'),30,cv::Size(640,480),1);
	m_cap.set(3, 640*2);
	m_cap.set(4, 480);

	cv::Mat l;
	cv::Mat r;
	cv::Mat mapL1;
	cv::Mat mapL2;
	cv::Mat mapR1;
	cv::Mat mapR2;
	bool is_ok = get_remap_mat(i_file, e_file, cv::Size(640,480), mapL1, mapL2, mapR1, mapR2);
	cv::Mat remap_frame;

	cv::VideoWriter video_capture = cv::VideoWriter("video_"+ video_usrname + ".avi",  CV_FOURCC('M', 'J', 'P', 'G' ), 30, cv::Size(640*2, 480), 1);
	cv::VideoWriter remap_video = cv::VideoWriter("remap_video_" + video_usrname + ".avi",  CV_FOURCC('M', 'J', 'P', 'G' ), 30, cv::Size(640*2, 480), 1);
	cv::Mat frame;
	if( video_capture.isOpened() && m_cap.isOpened() ){
		for(;;){
			m_cap >> frame;
			video_capture << frame;
			cv::imshow("stereo camera", frame);

			l = splitMat(frame, true);
			r = splitMat(frame, false);
			remap( l,  l, mapL1, mapL2, cv::INTER_LINEAR );
			remap( r, r, mapR1, mapR2, cv::INTER_LINEAR );
			remap_frame = combineTwoImg(l, r);

			cv::imshow("remap stereo camera", remap_frame);
			remap_video << remap_frame;
			int key = cv::waitKey(1);
			if(key == 'q')
				break;
		}
	}
	video_capture.release();
	remap_video.release();
	m_cap.release();
}

void CLRStereoCamera::calc_disp_by_calibFiles(
			const cv::Mat& l_src, const cv::Mat& r_src,
			cv::Mat& l_remap, cv::Mat& r_remap, cv::Mat& disp)
{
	remap(l_src, l_remap, m_mapL1, m_mapL2, cv::INTER_LINEAR);
	remap(r_src, r_remap, m_mapR1, m_mapR2, cv::INTER_LINEAR);
	cv::Mat smallL;
	cv::Mat smallR;
	cv::resize(l_remap, l_remap, cv::Size(320,240));
	cv::resize(r_remap, r_remap, cv::Size(320,240));
	//sgbm(l_remap, r_remap, disp);

/*
    static auto asgbm =
    //cv::StereoSGBM::create(   0,   //mindisp
    cv::StereoSGBM(   0,   //mindisp
                   48,   //numdisp
                   5,   //SADWindow
                   25,   //P1
                   50,   //P2
                   1,   //dispdiffmax
                   63,   //prefiltercap
                   40,   //uniqueness
                   200,   //speckle window size
                   2   //speckle range
                   );
*/
	sgbm(l_remap, r_remap, disp);

	 static cv::Mat disp8;
    disp.convertTo( disp8, CV_8U, 255.0/(48*16) );

    cv::applyColorMap( disp8, disp, cv::COLORMAP_JET );
    cv::resize(disp, disp, cv::Size(752,480));

    //cv::applyColorMap( disp, disp, cv::COLORMAP_JET );
}
