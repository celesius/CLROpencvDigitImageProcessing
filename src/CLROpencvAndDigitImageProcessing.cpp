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
#include "IFVRBlob.hpp"
#include "groundFloorSample/groundFloorSample.h"
#include "groundFloorSample/IFVRGroundFloor.h"
#include "CLRCVHelper/CLRCVHelper.hpp"
#include "IFVRAvoidances/IFVRAvoidances.h"

using namespace std;
using namespace cv;
//#define ONLYDISPLAY
#define COLOR
#define DISPARITY
//#define CALIB_STEREO
//#define CALIB_MONO
//#define CALIB_MONO_AFTERSTEREO
//#define BLOB
//#define TESTGROUND
//#define TESTAVOID
//#define HSVGRAY

void onMouse(int event,int x,int y,int flags,void* param );
PPoint pp[16];

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

void WAD_my(cv::Mat bgr)
{
	vector<cv::Mat> imageRGB;
	split(bgr, imageRGB);
	double R, G, B;
	B = mean(imageRGB[0])[0];
	G = mean(imageRGB[1])[0];
	R = mean(imageRGB[2])[0];
	double KR, KG, KB;
	KB = (R + G + B) / (3 * B);
	KG = (R + G + B) / (3 * G);
	KR = (R + G + B) / (3 * R);
	//调整RGB三个通道各自的值
	imageRGB[0] = imageRGB[0] * KB*2;
	imageRGB[1] = imageRGB[1] * KG*2;
	imageRGB[2] = imageRGB[2] * KR*2;
	merge(imageRGB, bgr);
}

void MyGammaCorrection(cv::Mat& src, cv::Mat& dst, float fGamma)
{
    CV_Assert(src.data);

    // accept only char type matrices
    CV_Assert(src.depth() != sizeof(uchar));

    // build look up table
    unsigned char lut[256];
    for( int i = 0; i < 256; i++ )
    {
        lut[i] = cv::saturate_cast<uchar>(pow((float)(i/255.0), fGamma) * 255.0f);
    }

    dst = src.clone();
    const int channels = dst.channels();
    switch(channels)
    {
        case 1:
            {

                cv::MatIterator_<uchar> it, end;
                for( it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++ )
                    //*it = pow((float)(((*it))/255.0), fGamma) * 255.0;
                    *it = lut[(*it)];

                break;
            }
        case 3:
            {

                cv::MatIterator_<cv::Vec3b> it, end;
                for( it = dst.begin<cv::Vec3b>(), end = dst.end<cv::Vec3b>(); it != end; it++ )
                {
                    //(*it)[0] = pow((float)(((*it)[0])/255.0), fGamma) * 255.0;
                    //(*it)[1] = pow((float)(((*it)[1])/255.0), fGamma) * 255.0;
                    //(*it)[2] = pow((float)(((*it)[2])/255.0), fGamma) * 255.0;
                    (*it)[0] = lut[((*it)[0])];
                    (*it)[1] = lut[((*it)[1])];
                    (*it)[2] = lut[((*it)[2])];
                }

                break;

            }
    }
}

#ifdef HSVGRAY

void hsv2gray(const cv::Mat& srcMat, cv::Mat& grayMat){
	int w = srcMat.cols;
	int h = srcMat.rows;
	grayMat = cv::Mat(h,w, CV_8UC1, cv::Scalar::all(0));

	for(int y = 0; y < h; y++){
		//const uchar* srcMatRows = srcMat.ptr<uchar>(y*3);
		uchar* grayMatRows = grayMat.ptr<uchar>(y);
		for(int x = 0; x < w; x++){
				const uchar grayPixel = srcMat.data[y*w*3 + x*3 + 0];
				grayMatRows[x] = grayPixel;
		}

	}

}

int main(){
	cv::Mat mat = cv::imread("/home/clover/workspace/orb_slam/ORB_SLAM2/pix_out3.6lv/mono7.png");

	cv::imshow("image", mat);
	cv::Mat gray;
	cv::cvtColor(mat, gray, CV_BGR2GRAY);
	cv::imshow("gray", gray);
	cv::Mat hsv;
	cv::cvtColor(mat, hsv, CV_BGR2YUV);
	cv::imshow("hsv", hsv);
	printf("%d %d\n", mat.depth(), mat.channels());
	printf("%d %d\n", hsv.depth(), hsv.channels());
	cv::Mat hsvGray;
	hsv2gray(hsv, hsvGray);
	cv::imshow("hsvGray", hsvGray);

	cv::waitKey(0);

}
#endif

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

#ifdef TESTAVOID
int main(){
	cv::Mat grayBlobMat;
	cv::Mat colorBlobMat;
	cv::Mat depthMat;
	std::string gBlobMat = "/home/clover/workspace/StereoDataset/blob/gray";
	std::string cBlobMat = "/home/clover/workspace/StereoDataset/blob/color";
	std::string dBlobMat = "/home/clover/workspace/StereoDataset/disparity_4_gray";

	int imageCnt = 101;
	IFVRAvoidances *avoid = new IFVRAvoidances();
	cv::VideoCapture cap = cv::VideoCapture(0);
	for(;;)
	{
		cap >> depthMat;
		cv::cvtColor(depthMat, depthMat, CV_BGR2GRAY);
		cv::imshow("image", depthMat);
		cv::waitKey(1);

	}
	/*
	for(;;){
		getMatByPath(gBlobMat, imageCnt, grayBlobMat);
		getMatByPath(cBlobMat, imageCnt, colorBlobMat);
		getMatByPath(dBlobMat, imageCnt, depthMat);

		cv::resize(depthMat, depthMat, cv::Size(grayBlobMat.cols, grayBlobMat.rows));

		cv::imshow("gray", grayBlobMat);
		cv::imshow("color", colorBlobMat);

		avoid->processingBlob(grayBlobMat, depthMat, imageCnt);

		imageCnt++;
		cv::waitKey(0);
	}
*/
	delete avoid;
}
#endif

#ifdef TESTGROUND
bool haveGround = false;
int main(){

	cv::Mat depth = cv::imread("/home/clover/workspace/StereoDataset/disparity_4_gray/image100.jpg");
    cv::cvtColor(depth, depth, CV_BGR2GRAY);
    //std::unique_ptr<IFVRGroundFloor> groundFloor (new IFVRGroundFloor);


    IFVRGroundFloor* groundFloor = new IFVRGroundFloor();
    groundFloor->setDetectMat(depth);
    cv::namedWindow("depth");
    cv::setMouseCallback("depth", onMouse, groundFloor);
    cv::imshow("depth", depth);
    cv::waitKey(0);

    std::string imagePath = "/home/clover/workspace/StereoDataset/disparity_4_gray";
    std::string imageSavePath = "/home/clover/workspace/StereoDataset/blob";

    char name[500];
    cv::Mat deGround;
    IFVRBlob *calcBlob = new IFVRBlob();
    cv::Mat grayBlob;
    cv::Mat colorBlob;
    int blobNum;
    int imageCnt = 101;
	IFVRAvoidances *avoid = new IFVRAvoidances();
    if(haveGround){
    	for(;;){
    		sprintf(name,"%s/image%d.jpg",imagePath.c_str(), imageCnt);

    		cv::Mat frame = cv::imread(name);
    		if(!frame.empty()){
    			groundFloor->deleteGround(frame, deGround);
    			cv::imshow("depth", frame);
    			cv::imshow("Ground", deGround);
    			calcBlob->getBlob(deGround, grayBlob, colorBlob, blobNum);

    			cv::Mat depthMat = frame;
    			cv::resize(depthMat, depthMat, cv::Size(grayBlob.cols, grayBlob.rows));

    			avoid->processingBlob(grayBlob, depthMat, imageCnt);

    			cv::imshow("color blob", colorBlob);
    			//cv::applyColorMap(grayBlob, grayBlob, COLORMAP_JET);
    			cv::imshow("gray blob", grayBlob);
    			sprintf(name,"%s/gray/image%d.jpg",imageSavePath.c_str(), imageCnt);
    			cv::imwrite(name, grayBlob);
    			sprintf(name,"%s/color/image%d.jpg",imageSavePath.c_str(), imageCnt);
    			cv::imwrite(name, colorBlob);

    			cv::waitKey(0);
    			imageCnt++;
    		}else{
    			break;
    		}
    	}
    }

}
#endif

#ifdef BLOB
int main(){
	std::string imagePath = "/home/clover/workspace/StereoDataset/disparity_4_gray";
	char imageName[500];
	IFVRBlob *blob = new IFVRBlob();
	cv::Mat blobMat;
	cv::Mat blobColorMat;
	cv::Mat image;
	int blobNumber;
	bool isRun = false;
	int readImageCnt = 90;
	for(;;){
		printf("image num = %d\n", readImageCnt);
		sprintf(imageName,"%s/image%d.jpg",imagePath.c_str(), readImageCnt);
		image = cv::imread(imageName);
		if(!image.empty()){
			blob->getBlob(image, blobMat, blobColorMat, blobNumber);
			printf("show\n");
			cv::imshow("blob", blobColorMat);
			cv::imshow("image",image);
			printf("in\n");

			int key = cv::waitKey(0);
			if(isRun)
				readImageCnt++;
			if(key != -1){
				if(key == 32){
					if(isRun)
						isRun = false;
					else
						isRun = true;
				}else if(key == 65361 || key == 65362){
					if(isRun)
						readImageCnt = readImageCnt - 2;
					else
						readImageCnt --;

					isRun = false;
				} else if(key == 65363 || key == 65364){
					if(isRun)
						readImageCnt --;
					else
						readImageCnt ++;

					isRun = false;
				}
				printf("key = %d\n", key);
			}
		}else{
			printf("image empty\n");
			break;

		}

	}
	delete blob;
}
#endif

void onMouse(int event,int x,int y,int flags,void* param )
{
    static int cnt = 0;
    static Vector4 v;
    //printf("The Event is : %d \n",event);
    //cv::Mat *a = (cv::Mat *)param;
    IFVRGroundFloor *gf = (IFVRGroundFloor *)param;
    cv::Mat frame = gf->getDetectMat();
    //cv::Mat frameDf;
    //cv::VideoCapture cap;
    //cap.isOpened()
    if(event == CV_EVENT_LBUTTONUP){
        printf("X %d | Y %d\n", x,y);
        printf("data = %d\n", frame.at<uchar>(y,x));

        if(cnt < 16){
            pp[cnt].x = x;
            pp[cnt].y = y;
        }
        cnt++;
        if(cnt == 16){
#ifdef TESTGROUND
        	haveGround = gf->detectGround(frame, frameDf, pp);
#endif
        }
    }
}
#ifndef HSVGRAY
#ifndef TESTAVOID
#ifndef TESTGROUND
#ifndef BLOB
int main(){
	DuoCalibrator calib(cv::Size(11,7)) ;
	IFVRStereoCamera* cam = new IFVRStereoCamera("/dev/video0", 752, 480, StereoCamera);
	//IFVRStereoCamera* cam = new IFVRStereoCamera("/dev/video0", 752, 480, MonoCamera);
	//IFVRStereoCamera* cam = new IFVRStereoCamera("/home/clover/workspace/CLROpencvDigitImageProcessing/dataSet/2017.3.14", 752, 480, StereoFile);
	//IFVRStereoCamera* camZed = new IFVRStereoCamera("/home/clover/workspace/CLROpencvDigitImageProcessing/dataSet/zeds/L/left_0111_4",
	//		"/home/clover/workspace/CLROpencvDigitImageProcessing/dataSet/zeds/R/right_0111_4",StereoFile);
	//IFVRStereoCamera* camBino = new IFVRStereoCamera("/home/clover/workspace/CLROpencvDigitImageProcessing/dataSet/2017.3.14/L00",
	//		"/home/clover/workspace/CLROpencvDigitImageProcessing/dataSet/2017.3.14/R00",StereoFile);

#ifdef DISPARITY
	//CLRStereoCamera *stereoCame = new CLRStereoCamera();

	//CLRStereoCamera *stereoCameBino = new CLRStereoCamera("/home/clover/workspace/CLROpencvDigitImageProcessing/extrinsicsDuoVGA_2017.3.13_6mm.yml",
	//		"/home/clover/workspace/CLROpencvDigitImageProcessing/intrinsicsDuoVGA_2017.3.13_6mm.yml", cv::Size(752, 480));//24块砖
	//CLRStereoCamera *stereoCameBino = new CLRStereoCamera("/home/clover/workspace/CLROpencvDigitImageProcessing/extrinsicsDuoVGA_2017.3.13_2.8mmNew.yml",
	//		"/home/clover/workspace/CLROpencvDigitImageProcessing/intrinsicsDuoVGA_2017.3.13_2.8mmNew.yml", cv::Size(752, 480));//
	//CLRStereoCamera *stereoCame = new CLRStereoCamera("/home/clover/workspace/CLROpencvDigitImageProcessing/extrinsicsDuoVGA.yml",
	//		"/home/clover/workspace/CLROpencvDigitImageProcessing/intrinsicsDuoVGA.yml", cv::Size(752, 480));//
	CLRStereoCamera *stereoCameBino = new CLRStereoCamera("/home/clover/TOOLS/opencv-2.4.13/samples/cpp/sC/extrinsics.yml",
		"/home/clover/TOOLS/opencv-2.4.13/samples/cpp/sC/intrinsics.yml", cv::Size(752, 480));
	#endif
	//IFVRStereoCamera* cam = new IFVRStereoCamera("1", 640, 480, MonoCamera);
#ifdef CALIB_MONO
#ifdef CALIB_MONO_AFTERSTEREO
	CLRStereoCamera *stereoCameBino = new CLRStereoCamera("/home/clover/TOOLS/opencv-2.4.13/samples/cpp/sC/extrinsics.yml",
			"/home/clover/TOOLS/opencv-2.4.13/samples/cpp/sC/intrinsics.yml", cv::Size(752, 480));//24块砖

#endif
	IFVRMonoCameraCalib *monoCamCalib = new IFVRMonoCameraCalib(cv::Size(11,7));
#endif
	cv::Mat drawChessMat;

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
	cv::Mat remapL;
	cv::Mat remapR;
	cv::Mat disp;
	int imageCnt = 0;
	int readImageCnt = 0;
	bool isRun = true;
	//char path[] = "/home/clover/workspace/CLROpencvDigitImageProcessing/dataSet/2017.3.14";
	char path[] = "/home/clover/workspace/StereoDataset/binoDepth";
	char name[500];
	//test mono
	cv::Mat monoMap1;
	cv::Mat monoMap2;
	cv::Mat monoMap1R;
	cv::Mat monoMap2R;
	//get_monoRemap_mat("./monoCalib_3.6mmLeftIRCut_para5_2017.3.27.yml", cv::Size(752, 480), monoMap1, monoMap2);
	//get_monoRemap_mat("./monoCalib_3.6mmRightIRCut_para5_2017.3.27.yml", cv::Size(752, 480), monoMap1R, monoMap2R);



	for(;;){
		//camZed->update(grayL, grayR, false, readImageCnt);
		cam->update(grayL, grayR, false, readImageCnt);
#ifdef COLOR
		cv::cvtColor(grayL, grayL, CV_BayerRG2BGR);
		cv::cvtColor(grayR, grayR, CV_BayerRG2BGR);
		WAD_my(grayL);
		WAD_my(grayR);
		cv::cvtColor(grayL, grayL, CV_BGR2GRAY);
		cv::cvtColor(grayR, grayR, CV_BGR2GRAY);
#endif
#ifdef ONLYDISPLAY

		cv::imshow("L A", grayL);
		MyGammaCorrection(grayL, grayL, 1.3);

		cv::imshow("L", grayL);
		cv::imshow("R", grayR);
		int key = cv::waitKey(1);
		if(key == 'q')
			break;
#endif
#ifdef CALIB_MONO
		monoCamCalib->resetImageSize(cv::Size(grayL.cols, grayL.rows));
#endif
#ifndef ONLYDISPLAY
#ifdef DISPARITY

		sprintf(name, "%s/02/image_0/%d.png",path,imageCnt);
		//cv::imwrite(name,grayL);
		sprintf(name, "%s/02/image_1/%d.png",path,imageCnt);
		//cv::imwrite(name,grayR);
		imageCnt++;

		//stereoCame->calc_disp_by_calibFiles(grayL, grayR,remapL, remapR, disp);

		//cv::imshow("dispZed", disp);

		//cam->update(grayL, grayR, false, readImageCnt);
		stereoCameBino->calc_disp_by_calibFiles(grayL, grayR,remapL, remapR, disp);
		cv::imshow("remapL",remapL);
		cv::imshow("remapR",remapR);
		cv::imshow("dispBino", disp);


		int key = cv::waitKey(1);
		if(isRun)
			readImageCnt++;
		if(key != -1){
			if(key == 32){
				if(isRun)
					isRun = false;
				else
					isRun = true;
			}else if(key == 65361 || key == 65362){
				if(isRun)
					readImageCnt = readImageCnt - 2;
				else
					readImageCnt --;

				isRun = false;
			} else if(key == 65363 || key == 65364){
				if(isRun)
					readImageCnt --;
				else
					readImageCnt ++;

				isRun = false;
			}
			//printf("key = %d\n", key);
		}

#endif
#ifdef CALIB_MONO
#ifdef CALIB_MONO_AFTERSTEREO
		stereoCameBino->get_remap_image(grayL, grayR);
		cv::imshow("stereo remaped Left", grayL);
		cv::imshow("stereo remaped Right", grayR);
#endif

		capOk = monoCamCalib->captureMat(grayL,true,drawChessMat);
		cv::putText(drawChessMat, capStatus, cv::Point(50,50),1,1,cv::Scalar(0,255,0));
		cv::imshow("chess", drawChessMat);
		//test mono
		//cv::remap(grayL, grayL, monoMap1, monoMap2, CV_INTER_LINEAR);
		//cv::imshow("image", grayL);
		if(calibOk){
			monoCamCalib->runCheckCalib(grayL, remap);
			cv::imshow("remap", remap);
		}
		int key = cv::waitKey(1);
		if(key == 'q')
			break;
		else if(key == 'c')
		{
			calibOk = monoCamCalib->runCalibAndSaveTo("./monoCalib_3.6_color.yml");
		}
		else if(key == 32){ //
			if(capOk){
				if(monoCamCalib->saveCurrentPoint()){
					sprintf(capStatus, "capture image : %d",capMonoImage++);
					sprintf(name,"mono%d.png",imageCnt++);
					cv::imwrite(name, grayL);
				}
			}
		}
		else if(key != -1)
		{
			printf("get key = %d\n",key);
		}
#endif
#ifdef CALIB_STEREO
		//cv::remap(grayL, grayL, monoMap1, monoMap2, CV_INTER_LINEAR);
		//cv::remap(grayR, grayR, monoMap1R, monoMap2R, CV_INTER_LINEAR);
		if(capImage)
			capImage = calibDuoRunByCaptureMat(grayL,grayR,&calib);
		else
		{
			if(cablib)
				checkCalibDuo(grayL,grayR,calib);
			else{
				std::cout << "Finished taking calibration images.\n";
				calib.calibrateWithCameraPara("monoCalib_3.6mmLeftIRCut_para5_2017.4.5.yml",
						"monoCalib_3.6mmRightIRCut_para5_2017.4.5.yml");
				//calib.calibrate();
				std::cout << "Stereo calibration completed.\n";
				cablib = true;
			}
		}
#endif
#endif
	}
	//delete cam;
}
#endif
#endif
#endif
#endif
