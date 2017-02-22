/*
 * CLRProcessingCommon.cpp
 *
 *  Created on: 2016年12月1日
 *      Author: clover
 */

#include "CLRProcessingCommon.h"

// Mat.clone() 是深拷贝!!!!
/*
cv::Mat splitMat(cv::Mat &frame, bool isLeft){
    int width = frame.cols;
    cv::Mat img;
    if(isLeft){
        img = frame(cv::Rect(0,0,width/2,frame.rows));
    }else{
        img = frame(cv::Rect(width/2,0,width/2,frame.rows));
    }
    //cv::resize(img,img,cv::Size(width/2,high/2));
    return img;
}
*/
