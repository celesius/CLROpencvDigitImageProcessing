/*
 * IFVRMonoCameraCalib.h
 *
 *  Created on: 2017年3月7日
 *      Author: clover
 */

#ifndef CALI_IFVRMONOCAMERACALIB_H_
#define CALI_IFVRMONOCAMERACALIB_H_

#include <vector>
#include <opencv2/opencv.hpp>


class IFVRMonoCameraCalib {
	std::vector<cv::Mat> m_captureMat;
	cv::Size m_chessboardSize;
	cv::Size m_imageSize;
	cv::Mat m_cameraMatrix;
	cv::Mat m_distCoeffs;

    cv::Mat m_map1;
	cv::Mat m_map2;

	std::vector<std::vector <cv::Point2f> > m_imagePoints;
	std::vector<cv::Point2f> m_oneFramePoints;
public:
	IFVRMonoCameraCalib(cv::Size chessSize);
	virtual ~IFVRMonoCameraCalib();
	bool captureMat(cv::Mat inputMat, bool isDrawChess, cv::Mat& drawChessMat);
	bool runCalibAndSaveTo(std::string calibFilePath);
	void resetImageSize(cv::Size imageSize);
	void runCheckCalib(const cv::Mat&orgMat, cv::Mat& checkMat);
	bool saveCurrentPoint();
};

#endif /* CALI_IFVRMONOCAMERACALIB_H_ */
