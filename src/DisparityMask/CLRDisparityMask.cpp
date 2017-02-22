/*
 * CLRDisparityMask.cpp
 *
 *  Created on: 2017年1月31日
 *      Author: clover
 */

#include "CLRDisparityMask.h"
#include <opencv2/opencv.hpp>

void mat_max_and_min(const cv::Mat& in_mat, uchar& max, uchar& min){
	max = 0;
	min = 255;
	uchar pixel = 0;
	for(int y = 0; y < in_mat.rows; y++){
		const uchar* in_mat_rows = in_mat.ptr<uchar>(y);
		for(int x = 0; x < in_mat.cols; x++){
			pixel = in_mat_rows[x];
			if(pixel < min)
				min = pixel;
			if(pixel > max)
				max = pixel;
		}
	}
}

void test_data(const cv::Mat& in_data, cv::Mat& out_data){
	out_data = cv::Mat(in_data.size(), CV_8UC1);
	for(int i = 0; i < in_data.cols* in_data.rows; i++){
		out_data.data[i] = in_data.data[i];
	}
}

void xy_2_xz(const cv::Mat& src_mat, cv::Mat &out_mat)
{
	int w = src_mat.cols;
	int h = src_mat.rows;
	out_mat = cv::Mat(255, w, CV_8UC1, cv::Scalar::all(0));
	for(int y =0; y < h; y++){
		const uchar *src_mat_rows = src_mat.ptr<uchar>(y);
		for(int x = 0; x < w; x++){
			uchar src_mat_pixel = src_mat_rows[x];
			uchar out_mat_data = out_mat.data[src_mat_pixel*w + x];
			if(out_mat_data == 0){
				if(y < 100)
					if(src_mat_pixel > 5)
						out_mat.data[src_mat_pixel*w + x]= 255;
			}
		}

	}
}

void find_xz_foreground(const cv::Mat& in_mat)
{
	cv::Mat out_mat;
	cv::Mat kernel = cv::Mat(3,3,CV_8UC1);
	kernel.data[0] = 0;
	kernel.data[1] = 1;
	kernel.data[2] = 0;
	kernel.data[3] = 1;
	kernel.data[4] = 1;
	kernel.data[5] = 1;
	kernel.data[6] = 0;
	kernel.data[7] = 1;
	kernel.data[8] = 0;

	cv::dilate(in_mat,out_mat,kernel);
	cv::erode(out_mat, out_mat, kernel);
	cv::imshow("dilate", out_mat);
}

/*
 * 查找深度图中前景物体
 * 仅利用高度和深度信息
 * 区分,不区分个数
 * 小于深度小于5的就是背景,低于max_hight的就是地面
 * */
void find_disparity_foreground(const cv::Mat in_mat, int max_hight, cv::Mat& out_mat)
{
	uchar pixel = 0;
	out_mat = cv::Mat(in_mat.size(), CV_8UC1, cv::Scalar::all(0));
	for(int y = 0; y < in_mat.rows - max_hight; y++){
		//const uchar* in_mat_rows = in_mat.ptr<uchar>(y);
		for(int x = 0; x < in_mat.cols; x++){
			//if(in_mat_rows)
			pixel = in_mat.data[y*in_mat.cols + x];
			if( pixel > 5)
				out_mat.data[y*in_mat.cols + x] = pixel;
		}
	}


}

CLRDisparityMask::CLRDisparityMask(std::string disparity_image_path, std::string color_image_path)
{
	// TODO Auto-generated constructor stub
	m_disparity_image_path = disparity_image_path;
	m_color_image_path = color_image_path;
}

CLRDisparityMask::~CLRDisparityMask() {
	// TODO Auto-generated destructor stub
}

void CLRDisparityMask::run_fusion(){
	m_disparity_image_path.append("/image94.jpg");
	cv::Mat d_mat = cv::imread(m_disparity_image_path,0);
	std::cout<<d_mat.channels() << std::endl;
	cv::Mat xz_mat;
	cv::Mat foreground;
	//d_mat.convertTo(d_mat, CV_8U, 255.0/20.0);
	//xy_2_xz(d_mat, xz_mat);
	//find_xz_foreground(xz_mat);
	//test_data(d_mat, xz_mat);

	find_disparity_foreground(d_mat,40, foreground);


	cv::applyColorMap(d_mat, d_mat, cv::COLORMAP_JET);
	cv::imshow("disparity mat", d_mat);
	cv::applyColorMap(foreground, foreground, cv::COLORMAP_JET);

	cv::imshow("foreground", foreground);

	cv::waitKey(0);

}
