#include <iostream>

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "DuoUtility.h"
#include "calibDuo.h"
#include "../CLRCVHelper/CLRCVHelper.hpp"
const cv::Scalar WHITE( 255, 255, 255 );
const cv::Scalar BLUE ( 255,   0,   0 );
const cv::Scalar GREEN(   0, 255,   0 );
const cv::Scalar RED  (   0,   0, 255 );

const std::string WINDOW_NAME = "Duo Calibration";

//
// Gain, Exposure, and LED valid 0-100
//
int       EXPOSURE     =  70;
const int EXPOSURE_MAX = 100;
int       GAIN         =   0;
const int GAIN_MAX     = 100;
int       LED          =  20;
const int LED_MAX      = 100;


//static void callbackExposure( int, void* ) { SetExposure(EXPOSURE); }
//static void callbackGain    ( int, void* ) { SetGain(GAIN);         }
//static void callbackLED     ( int, void* ) { SetLED(LED);           }



static void createTrackbars()
{
//  cv::createTrackbar( "Exposure", WINDOW_NAME,
//                      &EXPOSURE, EXPOSURE_MAX, callbackExposure );
//  cv::createTrackbar( "Gain", WINDOW_NAME,
//                      &GAIN, GAIN_MAX, callbackGain );
//  cv::createTrackbar( "LED", WINDOW_NAME,
//                      &LED, LED_MAX, callbackLED );
}


//
// After calibration, this function will help visualize the quality of the
// undistortion and rectification by drawing horizonatl lines across the
// side-by-side left and right images
//
static void drawLines( cv::Mat& display )
{
  const int rows = display.rows;
  const int cols = display.cols;

  const int numLines = 24;

  const int step = rows/numLines;

  for( int i = 1; i < numLines; ++i )
  {
    cv::line( display,
              cv::Point(    0, i*step ),
              cv::Point( cols, i*step ),
              GREEN );
  }
}

/*
cv::Mat splitMat(cv::Mat &frame, bool isLeft){
    int width = frame.cols;
    if(isLeft){
        return frame(cv::Rect(0,0,width/2,frame.rows));
    }else{
        return frame(cv::Rect(width/2,0,width/2,frame.rows));
    }
}
*/

int calibDuoRun( int cameraID )
{
    //printf( "DUOLib Version:       v%s\n", GetLibVersion() );
    
    //
    // Open DUO camera and start capturing
    //
    cv::VideoCapture cap(cameraID);
    //if( !OpenDUOCamera( WIDTH_VGA, HEIGHT_VGA, FPS ) )
    if(!cap.isOpened())
    {
        printf( "Could not open DUO camera\n" );
        return 0;
    }
    
    //
    // Set initial exposure, gain, and LED power
    //
    //SetExposure( EXPOSURE );
    //SetGain( GAIN );
    //SetLED( LED );
    
    cv::namedWindow( WINDOW_NAME, CV_GUI_NORMAL | CV_WINDOW_NORMAL );
    
    createTrackbars();
    
    cv::Mat left  = cv::Mat( VGA, CV_8U );
    cv::Mat right = cv::Mat( VGA, CV_8U );
    
    cv::Mat display = cv::Mat::zeros( VGA.height, 2*VGA.width, CV_8UC3 );
    
    cv::Mat leftDisplay  = display.colRange( 0, VGA.width );
    cv::Mat rightDisplay = display.colRange( VGA.width, 2*VGA.width );
    
    const cv::Size boardSize( 11, 7 );
    
    DuoCalibrator calibDuo( boardSize );
    
    std::cout << "Press a key to begin taking calibration images.\n";
    
    bool isActive = true;
    cv::Mat frame;
    
    while( isActive )
    {
        //
        // Capture DUO frame
        //
        //PDUOFrame pFrameData = GetDUOFrame();
        cap >> frame;
        //if( pFrameData == nullptr ) continue;
        
        //
        // Set the image data
        //
        //left.data  = (uint8_t*) pFrameData->leftData;
        //right.data = (uint8_t*) pFrameData->rightData;
        
        cv::cvtColor(frame, frame, CV_BGR2GRAY);
        left = splitMat(frame, true);
        right = splitMat(frame, false);
        
        static std::vector<cv::Point2f> leftPts;
        static std::vector<cv::Point2f> rightPts;
        
        calibDuo.processFrame( left, right, leftPts, rightPts );
        
        //cv::imshow("lll", left);
        //cv::imshow("rrr", right);
        //cv::waitKey(0);
        
        cv::cvtColor( left,  leftDisplay,  cv::COLOR_GRAY2BGR );
        cv::cvtColor( right, rightDisplay, cv::COLOR_GRAY2BGR );
        
        //cv::cvtColor( left,  left,  CV_BGR2GRAY);//cv::COLOR_GRAY2BGR );
        
        const bool foundL = leftPts.size()  == boardSize.area();
        const bool foundR = rightPts.size() == boardSize.area();
        
        cv::drawChessboardCorners( leftDisplay,  boardSize, leftPts,  foundL );
        cv::drawChessboardCorners( rightDisplay, boardSize, rightPts, foundR );
        
        std::stringstream ss;
        ss << "Press any key to capture a frame, ESC to begin calibration";
        
        display = frame;
        
        cv::putText( display,
                    ss.str(),
                    cv::Point( 10, 30 ),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.75,
                    WHITE );
        
        ss.str("");
        ss << "# Image Sets = " << calibDuo.getNumImageSets();
        
        cv::putText( display,
                    ss.str(),
                    cv::Point( 10, 60 ),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.75,
                    WHITE );
        
        cv::imshow( WINDOW_NAME, display );
        //cv::imshow("frame", frame);
        
        const int key = cv::waitKey(1);
        
        if( key >= 0 )
        {
            if( key == 27 )
            {
                //
                // Quit the loop with ESC
                //
                isActive = false;
            }
            
            //
            // Any key press is a command to keep a calibration pair
            //
            printf("key down\n");
            calibDuo.keepMostRecent();
        }
    }
    
    std::cout << "Finished taking calibration images.\n";
    
    calibDuo.calibrate();
    
    std::cout << "Stereo calibration completed.\n";
    
    const std::string DISP_WINDOW_NAME( "Disparity" );
    
    cv::namedWindow( DISP_WINDOW_NAME, CV_GUI_NORMAL | CV_WINDOW_NORMAL );
    
    isActive = true;
    
    int frameCnt = 0;
    char frameName[200] = {0};
    while( isActive )
    {
        //
        // Capture DUO frame
        //
        //PDUOFrame pFrameData = GetDUOFrame();
        cap >> frame;
        cv::cvtColor(frame, frame, CV_BGR2GRAY);
        sprintf(frameName, "/Users/clover/workspace/XCodeCPP/CLRStereoMatch/capStereoImage/img%04d.jpg",frameCnt);
        cv::imwrite(frameName, frame);
        //display = frame;
        
        //if( pFrameData == nullptr ) continue;
        
        //
        // Set the image data
        //
        //left.data  = (uint8_t*) pFrameData->leftData;
        //right.data = (uint8_t*) pFrameData->rightData;
        left = splitMat(frame, true);
        right = splitMat(frame, false);
        
        const cv::Mat& newLeft  = calibDuo.undistortAndRectifyLeft( left );
        const cv::Mat& newRight = calibDuo.undistortAndRectifyRight( right );
        
        cv::cvtColor( newLeft,  leftDisplay,  cv::COLOR_GRAY2BGR );
        cv::cvtColor( newRight, rightDisplay, cv::COLOR_GRAY2BGR );
        
        display = combineTwoImg(leftDisplay, rightDisplay);
        
        
        drawLines( display );
        
        cv::putText( display,
                    "Press ESC to terminate",
                    cv::Point( 10, 30 ),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.75,
                    WHITE );
        
        cv::imshow("display", display);
        //cv::imshow( "L", newLeft );
        //cv::imshow("R", newRight);
        
        const cv::Mat& disp = calibDuo.getDisparity( newLeft, newRight );
        
        sprintf(frameName, "/Users/clover/workspace/XCodeCPP/CLRStereoMatch/capDispImage/img%04d.jpg",frameCnt);
        cv::imwrite(frameName, disp);
        frameCnt++;
        
        cv::imshow( DISP_WINDOW_NAME, disp );
        
        if( cv::waitKey( 5 ) == 27 )
        {
            //
            // Terminate the program with ESC
            //
            isActive = false;
        }
    }
    
    return 0;
}

void initCalibDuo(cv::Size boardSize, DuoCalibrator *calib)
{
	calib = new DuoCalibrator(boardSize);
}

bool checkCalibDuo(cv::Mat& left, cv::Mat& right, DuoCalibrator& calibDuo)
{
        //
        // Capture DUO frame
        //
        //PDUOFrame pFrameData = GetDUOFrame();
        //cap >> frame;
        //cv::cvtColor(frame, frame, CV_BGR2GRAY);
        //sprintf(frameName, "/Users/clover/workspace/XCodeCPP/CLRStereoMatch/capStereoImage/img%04d.jpg",frameCnt);
        //cv::imwrite(frameName, frame);
        //display = frame;

        //if( pFrameData == nullptr ) continue;

        //
        // Set the image data
        //
        //left.data  = (uint8_t*) pFrameData->leftData;
        //right.data = (uint8_t*) pFrameData->rightData;
        //left = splitMat(frame, true);
        //right = splitMat(frame, false);

        const cv::Mat& newLeft  = calibDuo.undistortAndRectifyLeft( left );
        const cv::Mat& newRight = calibDuo.undistortAndRectifyRight( right );

        cv::Mat leftDisplay;
        cv::Mat rightDisplay;

        cv::cvtColor( newLeft,  leftDisplay,  cv::COLOR_GRAY2BGR );
        cv::cvtColor( newRight, rightDisplay, cv::COLOR_GRAY2BGR );

        cv::Mat display = combineTwoImg(leftDisplay, rightDisplay);

        drawLines( display );

        cv::putText( display,
                    "Press ESC to terminate",
                    cv::Point( 10, 30 ),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.75,
                    WHITE );

        cv::imshow("display", display);
        //cv::imshow( "L", newLeft );
        //cv::imshow("R", newRight);

        const cv::Mat& disp = calibDuo.getDisparity( newLeft, newRight );
/*
        sprintf(frameName, "/Users/clover/workspace/XCodeCPP/CLRStereoMatch/capDispImage/img%04d.jpg",frameCnt);
        cv::imwrite(frameName, disp);
        frameCnt++;
*/
        cv::imshow( "disp", disp );

        if( cv::waitKey( 5 ) == 27 )
        {
            //
            // Terminate the program with ESC
            //
            return false;
        }
        return true;

}


bool calibDuoRunByCaptureMat( cv::Mat& lMat, cv::Mat& rMat, DuoCalibrator *calibDuo)
{
	//
	// Capture DUO frame
	//
	//PDUOFrame pFrameData = GetDUOFrame();

	//cap >> frame;
	//if( pFrameData == nullptr ) continue;

	//
	// Set the image data
	//
	//left.data  = (uint8_t*) pFrameData->leftData;
	//right.data = (uint8_t*) pFrameData->rightData;

	//cv::cvtColor(frame, frame, CV_BGR2GRAY);
	//left = splitMat(frame, true);
	//right = splitMat(frame, false);

	cv::Mat left = lMat;
	cv::Mat right = rMat;
	cv::Mat leftDisplay;
	cv::Mat rightDisplay;
	cv::Mat frame = combineTwoImg(left, right);
	//cv::Mat display = frame.clone();
	//cv::cvtColor(display, display, CV_GRAY2BGR);
	//cv::cvtColor(left, left, CV_BGR2GRAY);
	//cv::cvtColor(right, right, CV_BGR2GRAY);


	static std::vector<cv::Point2f> leftPts;
	static std::vector<cv::Point2f> rightPts;

	calibDuo->processFrame( left, right, leftPts, rightPts );

	//cv::imshow("lll", left);
	//cv::imshow("rrr", right);
	//cv::waitKey(0);

	cv::cvtColor( left,  leftDisplay,  cv::COLOR_GRAY2BGR );
	cv::cvtColor( right, rightDisplay, cv::COLOR_GRAY2BGR );

	//cv::cvtColor( left,  left,  CV_BGR2GRAY);//cv::COLOR_GRAY2BGR );

	const bool foundL = leftPts.size()  == calibDuo->m_boardSize.area();//  boardSize.area();
	const bool foundR = rightPts.size() == calibDuo->m_boardSize.area();//  boardSize.area();

	cv::drawChessboardCorners( leftDisplay, calibDuo->m_boardSize , leftPts,  foundL );
	cv::drawChessboardCorners( rightDisplay,calibDuo->m_boardSize , rightPts, foundR );
	cv::Mat display = combineTwoImg(leftDisplay, rightDisplay);

	std::stringstream ss;
	ss << "Press any key to capture a frame, ESC to begin calibration";


	cv::putText( display,
			ss.str(),
			cv::Point( 10, 30 ),
			cv::FONT_HERSHEY_SIMPLEX,
			0.75,
			WHITE );

	ss.str("");
	ss << "# Image Sets = " << calibDuo->getNumImageSets();

	cv::putText( display,
			ss.str(),
			cv::Point( 10, 60 ),
			cv::FONT_HERSHEY_SIMPLEX,
			0.75,
			WHITE );

	cv::imshow( WINDOW_NAME, display );
	//cv::imshow("frame", frame);

	const int key = cv::waitKey(1);

	if( key >= 0 )
	{
		if( key == 27 )
		{
			//
			// Quit the loop with ESC
			//
			//isActive = false;
			return false;
		}

		//
		// Any key press is a command to keep a calibration pair
		//
		printf("key down\n");
		calibDuo->keepMostRecent();
		return true;
	}
	return true;
}


int calib_image_by_file(std::string file_path, cv::Size boardSize)
{
    cv::Mat frame;
    cv::Mat left;
    cv::Mat right;
    cv::Mat display;

    DuoCalibrator calibDuo( boardSize );
   
    cv::Mat leftDisplay;
    cv::Mat rightDisplay;
            
    int cnt = 0;
    char image_name[50] = {0};
    for(;;)
    {
        sprintf(image_name, "%s/img%04d.jpg", file_path.c_str(),cnt++);
        frame = cv::imread(image_name);
        
        if(!frame.empty()){
            //
            // Capture DUO frame
            //
            //PDUOFrame pFrameData = GetDUOFrame();
            //cap >> frame;
            //if( pFrameData == nullptr ) continue;
            
            //
            // Set the image data
            //
            //left.data  = (uint8_t*) pFrameData->leftData;
            //right.data = (uint8_t*) pFrameData->rightData;
            
            cv::cvtColor(frame, frame, CV_BGR2GRAY);
            left = splitMat(frame, true);
            right = splitMat(frame, false);
            
            static std::vector<cv::Point2f> leftPts;
            static std::vector<cv::Point2f> rightPts;
            
            calibDuo.processFrame( left, right, leftPts, rightPts );
            
            //cv::imshow("lll", left);
            //cv::imshow("rrr", right);
            //cv::waitKey(0);
            
            cv::cvtColor( left,  leftDisplay,  cv::COLOR_GRAY2BGR );
            cv::cvtColor( right, rightDisplay, cv::COLOR_GRAY2BGR );
            
            //cv::cvtColor( left,  left,  CV_BGR2GRAY);//cv::COLOR_GRAY2BGR );
            
            const bool foundL = leftPts.size()  == boardSize.area();
            const bool foundR = rightPts.size() == boardSize.area();
            
            cv::drawChessboardCorners( leftDisplay,  boardSize, leftPts,  foundL );
            cv::drawChessboardCorners( rightDisplay, boardSize, rightPts, foundR );
            
            std::stringstream ss;
            ss << "Press any key to capture a frame, ESC to begin calibration";
            
            display = frame;
            
            cv::putText( display,
                        ss.str(),
                        cv::Point( 10, 30 ),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.75,
                        WHITE );
            
            ss.str("");
            ss << "# Image Sets = " << calibDuo.getNumImageSets();
            
            cv::putText( display,
                        ss.str(),
                        cv::Point( 10, 60 ),
                        cv::FONT_HERSHEY_SIMPLEX,
                        0.75,
                        WHITE );
            
            cv::imshow( WINDOW_NAME, display );
            cv::imshow("frame", leftDisplay);
            
            
            const int key = cv::waitKey(0);
            
            if( key >= 0 )
            {
                if( key == 27 )
                {
                    //
                    // Quit the loop with ESC
                    //
                    //isActive = false;
                    break;
                }
                
                //
                // Any key press is a command to keep a calibration pair
                //
                printf("key down\n");
                calibDuo.keepMostRecent();
            }
        }else{
            std::cerr<<"read image err"<<std::endl;
            break;
        }
    }
    
    std::cout << "Finished taking calibration images.\n";
    
    calibDuo.calibrate();
    
    std::cout << "Stereo calibration completed.\n";
    
    return 0;
}

