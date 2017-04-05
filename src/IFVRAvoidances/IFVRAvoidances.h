/*
 * IFVRAvoidances.h
 *
 *  Created on: 2017年3月16日
 *      Author: clover
 */

#ifndef IFVRAVOIDANCES_IFVRAVOIDANCES_H_
#define IFVRAVOIDANCES_IFVRAVOIDANCES_H_
#include "opencv2/opencv.hpp"

enum direction{
	TurnLeft,
	TurnRight,
	Holding
};

typedef direction Direction;

class IFVRAvoidances {
	cv::Mat m_blobMat;
	cv::Mat m_depthMat;
public:
	IFVRAvoidances();
	virtual ~IFVRAvoidances();
	Direction processingBlob(const cv::Mat& blobMat, const cv::Mat& depthMat, int blobCnt);
};

#endif /* IFVRAVOIDANCES_IFVRAVOIDANCES_H_ */
