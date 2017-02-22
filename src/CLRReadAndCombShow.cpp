/*
 * CLRReadAndCombShow.cpp
 *
 *  Created on: 2017年1月7日
 *      Author: clover
 */

#include "CLRReadAndCombShow.h"
#include "CLRCVHelper/CLRCVHelper.hpp"

CLRReadAndCombShow::CLRReadAndCombShow(std::string disparity_1_image_path,
		std::string disparity_2_image_path,
		std::string rgb_image_path)
{
	// TODO Auto-generated constructor stub
	m_disp1_image_path = disparity_1_image_path;
	m_disp2_image_path = disparity_2_image_path;
	m_rgb_image_path = rgb_image_path;
}

CLRReadAndCombShow::~CLRReadAndCombShow() {
	// TODO Auto-generated destructor stub
}


void CLRReadAndCombShow::show(int delay)
{
	char name[100] = {0};
	int cnt = 0;
	cv::Mat disp1_mat;
	cv::Mat disp2_mat;
	cv::Mat color_mat;
	cv::Mat comb_mat;
	int user_delay = delay;
	for(;;){
		sprintf(name, "%s/%06d.png",m_rgb_image_path.c_str(),cnt);;
		color_mat = cv::imread(name);
		if(color_mat.empty()){
			cnt = 0;
			continue;
		}
		sprintf(name, "%s/%06d.png",m_disp1_image_path.c_str(),cnt);;
		disp1_mat = cv::imread(name);
		sprintf(name, "%s/%06d.png",m_disp2_image_path.c_str(),cnt);;
		disp2_mat = cv::imread(name);

		cnt++;
		cv::resize(disp1_mat, disp1_mat, cv::Size(disp1_mat.cols, disp1_mat.rows-20));
		cv::resize(disp2_mat, disp2_mat, disp1_mat.size());
		cv::resize(color_mat, color_mat, disp1_mat.size());

		comb_mat = combineTwoImgUpDown(color_mat, disp1_mat, disp2_mat);
		cv::imshow("video", comb_mat);

		int key = cv::waitKey(user_delay);
		if(key == '1')
			user_delay = 0;
		else if(key == '2')
			user_delay = delay;
		else if(key == 65361 || key == 65362){
			if(cnt > 2)
			cnt = cnt - 2;
			user_delay = 0;
		}
		else if(key == 'q')
			break;
		else
			printf("key number = %d\n", key);

	}
}

