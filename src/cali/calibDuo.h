//
//  calibDuo.h
//  CLRStereoMatch
//
//  Created by 蒋博 on 2016/11/8.
//  Copyright © 2016年 clover. All rights reserved.
//

#ifndef calibDuo_h
#define calibDuo_h

int calibDuoRun( int cameraID );
int calib_image_by_file(std::string file_path, cv::Size boardSize);
#endif /* calibDuo_h */
