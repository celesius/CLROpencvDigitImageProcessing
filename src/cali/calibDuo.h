//
//  calibDuo.h
//  CLRStereoMatch
//
//  Created by 蒋博 on 2016/11/8.
//  Copyright © 2016年 clover. All rights reserved.
//

#ifndef calibDuo_h
#define calibDuo_h

#include "DuoCalibrator.h"
int calibDuoRun( int cameraID );

void initCalibDuo(cv::Size boardSize, DuoCalibrator  *calib);

bool calibDuoRunByCaptureMat( cv::Mat& lMat, cv::Mat& rMat, DuoCalibrator *calibDuo);

bool checkCalibDuo(cv::Mat& left, cv::Mat& right, DuoCalibrator& calibDuo);

int calib_image_by_file(std::string file_path, cv::Size boardSize);
#endif /* calibDuo_h */
