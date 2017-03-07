//============================================================================
// Name        : CLROpencvAndDigitImageProcessing.cpp
// Author      : clover
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <opencv2/opencv.hpp>
#include "StereoCamera/CLRStereoCamera.h"
#include "cali/calibDuo.h"
#include "CLRReadAndCombShow.h"
#include "DisparityMask/CLRDisparityMask.h"
#include "IFVRStereoCamera/IFVRStereoCamera.h"
#include "cali/IFVRMonoCameraCalib.h"
using namespace std;
using namespace cv;

void show_test_mat(){
	Mat frame = Mat(256,256,CV_8UC1);
	int rows = frame.rows;
	int cols = frame.cols;
	for(int i = 0;i<rows;i++){
		uchar *dataCols = frame.ptr<uchar>(i);
		for(int j=0;j<cols;j++){
			dataCols[j] = j;
		}
	}
	imshow("Image", frame);
	waitKey(0);
}

#if 0

int main() {
	//CLRStereoCamera *stereo_camera = new CLRStereoCamera(1);
	//stereo_camera->capture_image(MANUAL_CAPTURE);
	//stereo_camera->capture_image_for_calib(MANUAL_CAPTURE, cv::Size(11,7));
	//calib_image_by_file("./", cv::Size(11, 7));
	//std::string e_file = "/home/clover/workspace/EclipseCHome/CLROpencvAndDigitImageProcessing/src/extrinsicsDuoVGA-2016-12-15-15-26-51.yml";
	//std::string i_file = "/home/clover/workspace/EclipseCHome/CLROpencvAndDigitImageProcessing/src/intrinsicsDuoVGA-2016-12-15-15-26-51.yml";
	//std::string e_file = "./extrinsicsDuoVGA_bigcamera_small_glass.yml";
	//std::string i_file = "./intrinsicsDuoVGA_bigcamera_small_glass.yml";
	//stereo_camera->show_stereo_camera_with_calibed(e_file, i_file);
	//stereo_camera->capture_video("DS2", e_file, i_file);
	std::string color = "/home/clover/imx6qNfsRoot/kitti_dataset/dataset/sequences/00/left";
	std::string disp1 = "/home/clover/imx6qNfsRoot/kitti_dataset/dataset/sequences/00/disparities_8path";
	std::string disp2 = "/home/clover/imx6qNfsRoot/kitti_dataset/dataset/sequences/00/disparities_4path";
	std::string zed_disp = "/home/clover/imx6qNfsRoot/zed_dataset_20170111/disparities";
	std::string zed_color = "/home/clover/imx6qNfsRoot/zed_dataset_20170111/left";
/*!
  CLRReadAndCombShow 是用于显示计算结果的程序

  */
	CLRReadAndCombShow *show = new CLRReadAndCombShow(disp1, disp2, color);
	show->show(1);

	//CLRDisparityMask *d_m = new CLRDisparityMask(zed_disp, zed_color);
	//d_m->run_fusion();
	//cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
#endif

#if 0
int main(){

	cv::VideoCapture cap = cv::VideoCapture(1);
	cv::Mat frame;
	if(cap.isOpened()){
		//cap >> frame;

		cap.retrieve(frame, 3);

		//cv::cvtColor(frame, frame, CV_BGR2YUV);

		std::cout<< frame.channels() <<std::endl;
		std::cout<< frame.type() <<std::endl;
		std::cout<< frame.depth() <<std::endl;

		cv::imshow("frame",frame);



		cv::waitKey(0);

	}else{
		std::cout<<"Camera open err" << std::endl;
	}

	return 0;

}
#endif

int main(){
	DuoCalibrator calib(cv::Size(11,7)) ;
	//IFVRStereoCamera* cam = new IFVRStereoCamera("/dev/video1", 752, 480, StereoCamera);
	IFVRStereoCamera* cam = new IFVRStereoCamera("1", 640, 480, MonoCamera);
	IFVRMonoCameraCalib *monoCamCalib = new IFVRMonoCameraCalib(cv::Size(11,7));
	cv::Mat drawChessMat;

	uint8_t* image;

	cv::Mat grayL;
	cv::Mat grayR;


	bool capImage = true;
	bool cablib = false;

	bool capOk = false;
	bool calibOk = false;

	cv::Mat remap;

	int capMonoImage = 0;
	char capStatus[100];
	sprintf(capStatus, "capture image : %d",capMonoImage);
	for(;;){
		cam->update(grayL, grayR);
		capOk = monoCamCalib->captureMat(grayL,true,drawChessMat);
		cv::putText(drawChessMat, capStatus, cv::Point(50,50),1,1,cv::Scalar(0,255,0));
		cv::imshow("chess", drawChessMat);
		if(calibOk){
			monoCamCalib->runCheckCalib(grayL, remap);
			cv::imshow("remap", remap);
		}
		int key = cv::waitKey(1);
		if(key == 'q')
			break;
		else if(key == 'c')
		{
			calibOk = monoCamCalib->runCalibAndSaveTo("");
		}
		else if(key == 32){ //
			if(capOk){
				if(monoCamCalib->saveCurrentPoint()){
					sprintf(capStatus, "capture image : %d",capMonoImage++);
				}
			}
		}
		else if(key != -1)
		{
			printf("get key = %d\n",key);
		}

		/*
		if(capImage)
			capImage = calibDuoRunByCaptureMat(grayL,grayR,&calib);
		else
		{
			if(cablib)
				checkCalibDuo(grayL,grayR,calib);
			else{
				std::cout << "Finished taking calibration images.\n";
				calib.calibrate();
				std::cout << "Stereo calibration completed.\n";
				cablib = true;
			}
		}
		*/
	}


/*
	cv::VideoCapture cap =  cv::VideoCapture(1);
	cv::Mat frame;
	for(;;)
	{
		cap >> frame;
		cv::imshow("video",frame);
		cv::waitKey(1);
	}
*/
	//delete cam;
}
