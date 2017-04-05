/*
 * IFVRAvoidances.cpp
 *
 *  Created on: 2017年3月16日
 *      Author: clover
 */

#include "IFVRAvoidances.h"

cv::Rect findBlobRect(cv::Mat blob, unsigned char blobNumber){
	int top = blob.rows;
	int bottom = 0;
	int left = blob.cols;
	int right = 0;
	cv::Mat testBlob = cv::Mat(blob.rows, blob.cols, CV_8UC1, cv::Scalar::all(0));
	for(int y = 0; y < blob.rows; y++){
		unsigned char* rowData = blob.ptr<unsigned char>(y);
		unsigned char* testData = testBlob.ptr<unsigned char>(y);
		for(int x = 0; x < blob.cols; x++){
			unsigned char pixel = rowData[x];
			if(pixel == blobNumber){
				testData[x] = 255;

				if(top > y)
					top = y;
				if(bottom < y)
					bottom = y;
				if(left > x)
					left = x;
				if(right < x)
					right = x;
			}
		}
	}
	cv::Rect blobRect = cv::Rect(left, top, right - left, bottom - top);
	std::cout<<blobRect <<std::endl;
	cv::imshow("test ccc", testBlob);
	return blobRect;
}

void getAverageMaxAndMinDepth(const cv::Mat& depthMat, const cv::Mat& blobMat, const unsigned char blobNumber,
		unsigned char& averageValue, unsigned char& maxValue, unsigned char& minValue){
	maxValue = 0;
	minValue = 255;
	float averageValueF = 0.0;
	int allDepthValue = 0;
	int pixelNum = 0;
	unsigned char depthData = 0;
	unsigned char blobData = 0;

	for(int y = 0; y < depthMat.rows; y++){
		const unsigned char* depthRows = depthMat.ptr<unsigned char>(y);
		const unsigned char* blobRows = blobMat.ptr<unsigned char>(y);
		for(int x = 0; x < depthMat.cols; x++){
			depthData = depthRows[x];
			blobData = blobRows[x];
			if(blobData == blobNumber){
				allDepthValue = allDepthValue + (int)depthData;
				pixelNum++;
				if(maxValue < depthData)
					maxValue = depthData;
				if(minValue > depthData)
					minValue = depthData;
			}
		}
	}
	averageValueF = allDepthValue/pixelNum;
	averageValue = (int)floorf(averageValueF);
}

void drawRectAndShow(const cv::Mat& inputMat, cv::Rect rect, bool isThrough)
{
	cv::Mat bgrMat;
	if(inputMat.channels() == 1){
		cv::cvtColor(inputMat, bgrMat, CV_GRAY2BGR);
	}else
		bgrMat = inputMat.clone();

	if(isThrough){

		cv::arrowedLine(bgrMat, cv::Point(bgrMat.cols/2, bgrMat.rows - 50), cv::Point(bgrMat.cols/2, bgrMat.rows - 100) ,cv::Scalar(0,255,0));

	}else{
		cv::rectangle(bgrMat, rect, cv::Scalar(255,0,0));
		if(rect.x + (rect.width/2) < bgrMat.cols/2){ //靠左，向右转
			cv::arrowedLine(bgrMat, cv::Point(bgrMat.cols/2 + 50, bgrMat.rows - 50), cv::Point(bgrMat.cols/2 + 200, bgrMat.rows - 50) ,cv::Scalar(0,255,0));
			//cv::arrowedLine(bgrMat, cv::Point(rect.x + (rect.width/2), rect.y + (rect.height * 3/4)), cv::Point(rect.x + rect.width, rect.y + (rect.height * 3/4)) ,cv::Scalar(0,255,0));
		}else{ //靠右，向左转
			cv::arrowedLine(bgrMat, cv::Point(bgrMat.cols/2 - 50, bgrMat.rows - 50), cv::Point(bgrMat.cols/2 - 200, bgrMat.rows - 50) ,cv::Scalar(0,255,0));
			//cv::arrowedLine(bgrMat, cv::Point(rect.x + (rect.width/2), rect.y + (rect.height * 3/4)), cv::Point(rect.x , rect.y + (rect.height * 3/4)) ,cv::Scalar(0,255,0));
		}
	}
	cv::imshow("rect show", bgrMat);
}

void testShowBlob(const cv::Mat&input , cv::Point point)
{
	cv::Mat colorMat = input.clone();
	cv::applyColorMap(colorMat, colorMat, 2);
	cv::drawMarker(colorMat, point, cv::Scalar(0,0,255));
	cv::imshow("test color", colorMat);
}

IFVRAvoidances::IFVRAvoidances() {
	// TODO Auto-generated constructor stub
	
}

IFVRAvoidances::~IFVRAvoidances() {
	// TODO Auto-generated destructor stub
}

Direction IFVRAvoidances::processingBlob(const cv::Mat& blobMat, const cv::Mat& depthMat, int blobCnt)
{
	if(blobMat.channels() != 1)
		cv::cvtColor(blobMat,m_blobMat, CV_BGR2GRAY );
	else
		m_blobMat = blobMat.clone();

	if(depthMat.channels() != 1)
		cv::cvtColor(depthMat, m_depthMat, CV_BGR2GRAY);
	else
		m_depthMat = depthMat;

	cv::Point matCenter = cv::Point(m_blobMat.cols/2, m_blobMat.rows/2);
	unsigned char matCenerData = m_blobMat.data[matCenter.y*m_blobMat.cols + matCenter.x];
	//unsigned char matCenerData = m_blobMat.at<unsigned char>(matCenter.x, matCenter.y);
	//printf("piex = %d\n", matCenerData);
	//testShowBlob(m_blobMat, matCenter);
	if(matCenerData!=0){
		cv::Rect blobRect = findBlobRect(m_blobMat, matCenerData);
		drawRectAndShow(m_depthMat, blobRect, false);
		return TurnLeft;
	}else{
		drawRectAndShow(m_depthMat, cv::Rect(0,0,0,0), true);
		return Holding;
	}
}
