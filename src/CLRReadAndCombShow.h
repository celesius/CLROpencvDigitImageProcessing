/*
 * CLRReadAndCombShow.h
 *
 *  Created on: 2017年1月7日
 *      Author: clover
 */

#ifndef CLRREADANDCOMBSHOW_H_
#define CLRREADANDCOMBSHOW_H_

#include <iostream>
class CLRReadAndCombShow {
private:
	std::string m_disp1_image_path;
	std::string m_disp2_image_path;
	std::string m_rgb_image_path;
public:
	CLRReadAndCombShow(std::string disparity_1_image_path, std::string disparity_2_image_path, std::string rgb_image_path);
	//CLRReadAndCombShow();
	virtual ~CLRReadAndCombShow();
	void show(int delay);
};

#endif /* CLRREADANDCOMBSHOW_H_ */
