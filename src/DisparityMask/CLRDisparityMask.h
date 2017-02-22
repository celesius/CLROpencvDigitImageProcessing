/*
 * CLRDisparityMask.h
 *
 *  Created on: 2017年1月31日
 *      Author: clover
 */

#ifndef DISPARITYMASK_CLRDISPARITYMASK_H_
#define DISPARITYMASK_CLRDISPARITYMASK_H_

#include <string>

class CLRDisparityMask {
	std::string m_disparity_image_path;
	std::string m_color_image_path;
public:
	CLRDisparityMask(std::string disparity_image_path, std::string color_image_path);
	virtual ~CLRDisparityMask();
	void run_fusion();
};

#endif /* DISPARITYMASK_CLRDISPARITYMASK_H_ */
