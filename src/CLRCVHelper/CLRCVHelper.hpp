//
//  CLRCVHelper.hpp
//  CLRStereoMatch
//
//  Created by 蒋博 on 2016/11/12.
//  Copyright © 2016年 clover. All rights reserved.
//

#ifndef CLRCVHelper_hpp
#define CLRCVHelper_hpp

#include <stdio.h>
#include <vector>
#include <opencv2/opencv.hpp>

#define PRJPATH "/Users/clover/workspace/XCodeCPP/CLRStereoMatch/"


//cc is CLRCV
enum clrcvFileType{
    CCIntrinsucFile,
    CCExtrinsicFile,
	CCMonoCamearFile
};

typedef clrcvFileType CLRCVFileType;

bool printMatInfo(const cv::Mat &srcMat);
cv::Mat combineTwoImg(const cv::Mat &srcMatA, const cv::Mat &srcMatB);
cv::Mat combineTwoImgUpDown(const cv::Mat &srcMatA, const cv::Mat &srcMatB, const cv::Mat& srcMatC);
cv::Mat splitMat(const cv::Mat srcImage, bool isLeftImage);
bool recodeVideo(std::string fileName, int cameraID);
void capImage(std::string savePath, int cameraID);
bool readYML(std::string fileName, CLRCVFileType ft, std::vector<cv::Mat> &getMat);

//bool get_remap_mat(std::string intrinsuc_file, std::string extrinsic_file, cv::Mat &mapL1, cv::Mat &mapL2, cv::Mat &mapR1, cv::Mat &mapR2);
/*
 remap( leftImg,  newLeft, m_mapL1, m_mapL2, cv::INTER_LINEAR );
 remap( rightImg, newRight, m_mapR1, m_mapR2, cv::INTER_LINEAR );
 Mat cImg = combineTwoImg(newLeft, newRight);
 */
bool get_remap_mat(std::string intrinsuc_file, std::string extrinsic_file, cv::Size image_size, cv::Mat &mapL1, cv::Mat &mapL2, cv::Mat &mapR1, cv::Mat &mapR2);
bool get_monoRemap_mat(std::string intrinsuc_file, cv::Size image_size, cv::Mat &map1, cv::Mat &map2);
//void trans_video_2_image(cv::)

bool getMatByPath(std::string path, int imageCnt, cv::Mat& image);

#endif /* CLRCVHelper_hpp */
