//
//  CLRCVHelper.cpp
//  CLRStereoMatch
//
//  常用的opencv算法小工具
//  Created by 蒋博 on 2016/11/12.
//  Copyright © 2016年 clover. All rights reserved.
//  
//

#include "CLRCVHelper.hpp"

using namespace cv;

Mat combineTwoImg(const Mat &srcMatA, const Mat &srcMatB){
    int maxWidth = (srcMatA.cols > srcMatB.cols) ? srcMatA.cols : srcMatB.cols;
    int maxHeight = (srcMatA.rows > srcMatB.rows) ? srcMatA.rows : srcMatB.rows;
    if(srcMatA.channels() != srcMatB.channels()){
        std::cerr<<" mat channel different，can't combine "<<std::endl;
        return Mat(0,0,0);
    }
    
    Mat cImg = Mat(maxHeight, maxWidth*2, CV_MAKETYPE(srcMatA.depth(), srcMatA.channels()));
    
    //std::hcon
   
    Mat roiA = cImg(Rect(0,0,srcMatA.cols, srcMatA.rows));
    Mat roiB = cImg(Rect(srcMatA.cols,0,srcMatB.cols, srcMatB.rows));
    
    cv::resize(srcMatA, roiA, roiA.size());
    
    cv::resize(srcMatB, roiB, roiB.size());
    
    return cImg;
}

cv::Mat combineTwoImgUpDown(const cv::Mat &srcMatA, const cv::Mat &srcMatB, const cv::Mat& srcMatC)
{
    int maxWidth = (srcMatA.cols > srcMatB.cols) ? srcMatA.cols : srcMatB.cols;
    int maxHeight = (srcMatA.rows > srcMatB.rows) ? srcMatA.rows : srcMatB.rows;
    if(srcMatA.channels() != srcMatB.channels()){
        std::cerr<<" mat channel different，can't combine "<<std::endl;
        return Mat(0,0,0);
    }

    Mat cImg = Mat(maxHeight*3, maxWidth, CV_MAKETYPE(srcMatA.depth(), srcMatA.channels()));

    //std::hcon

    Mat roiA = cImg(Rect(0,0,srcMatA.cols, srcMatA.rows));
    Mat roiB = cImg(Rect(0,srcMatA.rows,srcMatB.cols, srcMatB.rows));
    Mat roiC = cImg(Rect(0,srcMatA.rows*2,srcMatC.cols, srcMatC.rows));

    cv::resize(srcMatA, roiA, roiA.size());
    cv::resize(srcMatB, roiB, roiB.size());
    cv::resize(srcMatC, roiC, roiC.size());

    return cImg;

}


Mat splitMat(const cv::Mat srcImage, bool isLeftImage)
{
    Mat spImg;
    if(isLeftImage)
        spImg = srcImage(cv::Rect(0,0,srcImage.cols/2,srcImage.rows));
    else
        spImg = srcImage(cv::Rect(srcImage.cols/2,0,srcImage.cols/2,srcImage.rows));
    return spImg;
}


bool printMatInfo(const Mat &srcMat){
    std::cout<<"Mat info:" <<std::endl;
    std::cout<<" channels = "<<srcMat.channels()<<std::endl;
    std::cout<<" depth    = "<<srcMat.depth()<<std::endl;
    return true;
}


bool recodeVideo(std::string fileName, int cameraID)
{
    VideoCapture cap(cameraID);
    if(cap.isOpened()){
        Mat frame;
        cv::Size frameSize = cv::Size(cap.get(CV_CAP_PROP_FRAME_WIDTH), cap.get(CV_CAP_PROP_FRAME_HEIGHT));
        VideoWriter vVideo(fileName, cap.get(CV_CAP_PROP_FOURCC), cap.get(CV_CAP_PROP_FPS), frameSize);
        //VideoWriter vVideo(fileName, CV_FOURCC('M','J','P','G'), cap.get(CV_CAP_PROP_FPS), frameSize);
        for (;;) {
            if(!cap.read(frame)){
                //vVideo.write(frame);
                vVideo << frame;
                imshow("recode previwe", frame);
                int key = waitKey(1);
                if(key == 'q'){
                    
                    break;
                }
            }
            else{
                std::cerr<<"Open vidwoWrite error : "<< cap.get(CV_CAP_PROP_FOURCC) <<std::endl;
                
                vVideo.release();
                cap.release();
                return false;
            }
        }
        vVideo.release();
    }else{
        cap.release();
        std::cerr<<"Opencv Camera: "<<cameraID<<" error"<<std::endl;
        return false;
    }
    cap.release();
    return true;
}

void capImage(std::string savePath, int cameraID){
    VideoCapture cap(cameraID);
    if(cap.isOpened()){
        Mat frame;
        int cnt = 0;
        char saveName[300] = {0};
        for (;;) {
            cap >> frame;
            //if(!cap.read(frame)){
            if(!frame.empty()){
                imshow("recode previwe", frame);
                sprintf(saveName, "%s/img%04d.jpg",savePath.c_str(),cnt++);
                //imwrite(saveName, frame);
                int key = waitKey(1);
                if(key == 'q'){
                    break;
                }
            }
            else{
                std::cerr<<"Open vidwoWrite error : "<< cap.get(CV_CAP_PROP_FOURCC) <<std::endl;
                cap.release();
            }
        }
    }else{
        cap.release();
        std::cerr<<"Opencv Camera: "<<cameraID<<" error"<<std::endl;
    }
    cap.release();
}

bool readYML(std::string fileName, CLRCVFileType ft, std::vector<cv::Mat> &getMatArray)
//bool readYML(std::string fileName, CLRCVFileType ft)
{
    FileStorage fs(fileName, CV_STORAGE_READ);
    if(!fs.isOpened()){
        std::cerr<<" Read File : "<<fileName<<"Error !"<<std::endl;
        return false;
    }
    
    if(ft == CCIntrinsucFile){
        Mat m1,m2,d1,d2;
        fs["M1"] >> m1;
        fs["M2"] >> m2;
        fs["D1"] >> d1;
        fs["D2"] >> d2;
        getMatArray.push_back(m1);
        getMatArray.push_back(d1);
        getMatArray.push_back(m2);
        getMatArray.push_back(d2);
    }else if(ft == CCExtrinsicFile){
        Mat R,T;
        fs["R"] >> R;
        fs["T"] >> T;
        getMatArray.push_back(R);
        getMatArray.push_back(T);
    }else{
        std::cerr<<" Read File TYPE err ! "<<std::endl;
        return false;
    }
    return true;
}

bool get_remap_mat(std::string intrinsuc_file, std::string extrinsic_file, cv::Size image_size, cv::Mat &mapL1, cv::Mat &mapL2, cv::Mat &mapR1, cv::Mat &mapR2)
{
    std::vector<cv::Mat> readIData;
    std::vector<cv::Mat> readEData;
    
    readYML(intrinsuc_file, CCIntrinsucFile, readIData);
    //for ( cv::Mat mm:readIData ) {
    //    std::cout<<mm<<std::endl;
    //}
    readYML(extrinsic_file, CCExtrinsicFile, readEData);
    
    //文件读出的mat
    Mat m_M1,m_D1,m_M2,m_D2,m_R,m_T;
    m_M1 = readIData[0];
    m_D1 = readIData[1];
    m_M2 = readIData[2];
    m_D2 = readIData[3];
    m_R = readEData[0];
    m_T = readEData[1];
    //计算结果
    Mat m_R1,m_P1,m_R2,m_P2,m_Q;
    
    //Size m_ImgSize(640,480);
    //Size m_ImgSize(1280,720);
    
    cv::stereoRectify( m_M1, m_D1, m_M2, m_D2,
                      image_size ,
                      m_R, m_T, m_R1, m_R2, m_P1, m_P2, m_Q,
                      CV_CALIB_ZERO_DISPARITY, 0 );
    
    std::cout << "Undistort Rectify\n";
   
    //Mat m_mapL1, m_mapL2, m_mapR1, m_mapR2;
    cv::initUndistortRectifyMap( m_M1, m_D1, m_R1, m_P1,
                                image_size, CV_16SC2,
                                mapL1, mapL2 );
    
    cv::initUndistortRectifyMap( m_M2, m_D2, m_R2, m_P2,
                                image_size, CV_16SC2,
                                mapR1, mapR2 );
    return true;
}
