
#include <iostream>
#include <iomanip>
#include <ctime>
#include <regex>
#include <sstream>

#include <opencv2/calib3d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>

#include "DuoCalibrator.h"

#include "../CLRCVHelper/CLRCVHelper.hpp"

static std::string now()
{
  auto tp = std::time( nullptr );

  std::stringstream ss;
  //ss << std::put_time( std::localtime( &tp ), "%F-%H-%M-%S" );
  std::string time = "1970";

  return time;//ss.str();
}


// Update the input string.
static void autoExpandEnvironmentVariables( std::string& text )
{
  static std::regex env( "\\$\\{([^}]+)\\}" );
  std::smatch match;
  while( std::regex_search( text, match, env ) )
  {
    const char * s = getenv( match[1].str().c_str() );
    const std::string var( s == NULL ? "" : s );
   // text.replace( match[0].first, match[0].second, var );

  }
}

// Leave input alone and return new string.
static std::string expandEnvironmentVariables( const std::string& input )
{
  std::string text = input;
  autoExpandEnvironmentVariables( text );
  return text;
}

const std::string dateTime = now();

//const std::string intrinsics = "cameraFiles/intrinsicsDuoVGA-" + dateTime + ".yml";
const std::string intrinsics = "intrinsicsDuoVGA.yml";

//const std::string extrinsics = "cameraFiles/extrinsicsDuoVGA-" + dateTime + ".yml";
const std::string extrinsics = "extrinsicsDuoVGA.yml";

DuoCalibrator::DuoCalibrator( const cv::Size& boardSize ):
		m_squareLength(1.900),m_boardSize(boardSize),m_imageSize(DUO_FULL)
{

	//m_squareLength = 2.533 ;  // in cm, but this could be changed to m or mm
	//m_boardSize = boardSize ; // inner corners (this project's "chessboard" is 9x6)
	//m_imageSize = VGA ;       // default resolution (could go as high as 752x480)

}


void DuoCalibrator::processFrame( const cv::Mat& left,
                                  const cv::Mat& right,
                                  std::vector<cv::Point2f>& leftPtsOut,
                                  std::vector<cv::Point2f>& rightPtsOut )
{
  leftPtsOut.clear();
  rightPtsOut.clear();

  detectChessboardPoints( left,  right, leftPtsOut, rightPtsOut );
  //detectCirclesGridPoints( left,  right, leftPtsOut, rightPtsOut );
}

void DuoCalibrator::calibrate()
{
	if( m_objectPts.size() < 10 )
	{
		std::cout << "Too few frames, unable to continue with calibration\n";
		exit(1);
	}

	//const auto calibDuoRoot = std::string(PRJPATH); //expandEnvironmentVariables( "${CALIBDUO_ROOT}/" );
	const auto calibDuoRoot = std::string("./"); //expandEnvironmentVariables( "${CALIBDUO_ROOT}/" );

	std::cout << "Stereo Calibrate...\n";

	const auto termCrit =
			//cv::TermCriteria( cv::TermCriteria::COUNT + cv::TermCriteria::EPS,
			cv::TermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,
					//30,      // iterations
					//1e-6 );  // change epsilon
					100,      // iterations
					1e-5 );  // change epsilon

	const double errorX =
			cv::stereoCalibrate( m_objectPts, m_imagePtsL, m_imagePtsR,
					m_M1, m_D1, m_M2, m_D2,
					m_imageSize,
					m_R, m_T, m_E, m_F,
					//CV_CALIB_RATIONAL_MODEL + CV_CALIB_SAME_FOCAL_LENGTH,
					//termCrit );
					termCrit ,
					CV_CALIB_FIX_ASPECT_RATIO +
					CV_CALIB_FIX_PRINCIPAL_POINT +
					CV_CALIB_ZERO_TANGENT_DIST +
					CV_CALIB_SAME_FOCAL_LENGTH +
					CV_CALIB_RATIONAL_MODEL );
	//CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);//不添加会变形

	std::cout << "Stereo Reprojection Error: " << errorX << std::endl;

	cv::FileStorage fsI( calibDuoRoot + intrinsics, cv::FileStorage::WRITE );

	if( fsI.isOpened() )
	{
		fsI << "DateTime"          << dateTime
				<< "BoardWidth"        << m_boardSize.width
				<< "BoardHeight"       << m_boardSize.height
				<< "SquareLength"      << m_squareLength
				<< "ImageWidth"        << m_imageSize.width
				<< "ImageHeight"       << m_imageSize.height
				<< "NumImagePairs"     << (int) m_objectPts.size()
				<< "ReprojectionError" << errorX;

		fsI << "M1" << m_M1
				<< "D1" << m_D1
				<< "M2" << m_M2
				<< "D2" << m_D2;

		fsI.release();
	}
	else
	{
		std::cout << "File <intrinsics>.yml could not be opened.\n";
		//exit(1);
	}

	std::cout << "Stereo Rectify...\n";

	cv::stereoRectify( m_M1, m_D1, m_M2, m_D2,
			m_imageSize,
			m_R, m_T, m_R1, m_R2, m_P1, m_P2, m_Q,
			CV_CALIB_ZERO_DISPARITY, 0 );

	std::cout << "Undistort Rectify\n";

	cv::initUndistortRectifyMap( m_M1, m_D1, m_R1, m_P1,
			m_imageSize, CV_16SC2,
			m_mapL1, m_mapL2 );

	cv::initUndistortRectifyMap( m_M2, m_D2, m_R2, m_P2,
			m_imageSize, CV_16SC2,
			m_mapR1, m_mapR2 );

	cv::FileStorage fsX( calibDuoRoot + extrinsics, cv::FileStorage::WRITE );

	if( fsX.isOpened() )
	{
		fsX << "DateTime"          << dateTime
				<< "BoardWidth"        << m_boardSize.width
				<< "BoardHeight"       << m_boardSize.height
				<< "SquareLength"      << m_squareLength
				<< "ImageWidth"        << m_imageSize.width
				<< "ImageHeight"       << m_imageSize.height
				<< "NumImagePairs"     << (int) m_objectPts.size()
				<< "ReprojectionError" << errorX;

		fsX << "R"  << m_R
				<< "T"  << m_T
				<< "R1" << m_R1
				<< "R2" << m_R2
				<< "P1" << m_P1
				<< "P2" << m_P2
				<< "Q"  << m_Q;

		fsX << "E" << m_E
				<< "F" << m_F;

		fsX.release();
	}
	else
	{
		std::cout << "File <extrinsics>.yml could not be opened.\n";
		//exit(2);
	}
}

void DuoCalibrator::calibrateWithCameraPara(std::string leftPara, std::string rightPara)
{
	if( m_objectPts.size() < 10 )
	{
		std::cout << "Too few frames, unable to continue with calibration\n";
		exit(1);
	}

	//get_monoRemap_mat(leftPara, m_imageSize, m_M1, m_D1);
	//get_monoRemap_mat(rightPara, m_imageSize, m_M2, m_D2);
	//std::cout<<m_M1<<std::endl;
	//std::cout<<m_M2<<std::endl;

	//const auto calibDuoRoot = std::string(PRJPATH); //expandEnvironmentVariables( "${CALIBDUO_ROOT}/" );
	const auto calibDuoRoot = std::string("./"); //expandEnvironmentVariables( "${CALIBDUO_ROOT}/" );

	std::cout << "Stereo Calibrate...\n";

	const auto termCrit =
			//cv::TermCriteria( cv::TermCriteria::COUNT + cv::TermCriteria::EPS,
			cv::TermCriteria( CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,
					//30,      // iterations
					//1e-6 );  // change epsilon
					100,      // iterations
					1e-5 );  // change epsilon

	const double errorX =
			cv::stereoCalibrate( m_objectPts, m_imagePtsL, m_imagePtsR,
					m_M1, m_D1, m_M2, m_D2,
					m_imageSize,
					m_R, m_T, m_E, m_F,
					//CV_CALIB_RATIONAL_MODEL + CV_CALIB_SAME_FOCAL_LENGTH,
					//termCrit );
					termCrit ,
					0);
					//CV_CALIB_FIX_INTRINSIC);
					//CV_CALIB_FIX_ASPECT_RATIO +
					//CV_CALIB_FIX_PRINCIPAL_POINT +
					//CV_CALIB_ZERO_TANGENT_DIST +
					//CV_CALIB_SAME_FOCAL_LENGTH +
					//CV_CALIB_RATIONAL_MODEL );
	//CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);//不添加会变形

	std::cout << "Stereo Reprojection Error: " << errorX << std::endl;

	cv::FileStorage fsI( calibDuoRoot + intrinsics, cv::FileStorage::WRITE );

	if( fsI.isOpened() )
	{
		fsI << "DateTime"          << dateTime
				<< "BoardWidth"        << m_boardSize.width
				<< "BoardHeight"       << m_boardSize.height
				<< "SquareLength"      << m_squareLength
				<< "ImageWidth"        << m_imageSize.width
				<< "ImageHeight"       << m_imageSize.height
				<< "NumImagePairs"     << (int) m_objectPts.size()
				<< "ReprojectionError" << errorX;

		fsI << "M1" << m_M1
				<< "D1" << m_D1
				<< "M2" << m_M2
				<< "D2" << m_D2;

		fsI.release();
	}
	else
	{
		std::cout << "File <intrinsics>.yml could not be opened.\n";
		//exit(1);
	}

	std::cout << "Stereo Rectify...\n";

	cv::stereoRectify( m_M1, m_D1, m_M2, m_D2,
			m_imageSize,
			m_R, m_T, m_R1, m_R2, m_P1, m_P2, m_Q,
			CV_CALIB_ZERO_DISPARITY, 0 );

	std::cout << "Undistort Rectify\n";

	cv::initUndistortRectifyMap( m_M1, m_D1, m_R1, m_P1,
			m_imageSize, CV_16SC2,
			m_mapL1, m_mapL2 );

	cv::initUndistortRectifyMap( m_M2, m_D2, m_R2, m_P2,
			m_imageSize, CV_16SC2,
			m_mapR1, m_mapR2 );

	cv::FileStorage fsX( calibDuoRoot + extrinsics, cv::FileStorage::WRITE );

	if( fsX.isOpened() )
	{
		fsX << "DateTime"          << dateTime
				<< "BoardWidth"        << m_boardSize.width
				<< "BoardHeight"       << m_boardSize.height
				<< "SquareLength"      << m_squareLength
				<< "ImageWidth"        << m_imageSize.width
				<< "ImageHeight"       << m_imageSize.height
				<< "NumImagePairs"     << (int) m_objectPts.size()
				<< "ReprojectionError" << errorX;

		fsX << "R"  << m_R
				<< "T"  << m_T
				<< "R1" << m_R1
				<< "R2" << m_R2
				<< "P1" << m_P1
				<< "P2" << m_P2
				<< "Q"  << m_Q;

		fsX << "E" << m_E
				<< "F" << m_F;

		fsX.release();
	}
	else
	{
		std::cout << "File <extrinsics>.yml could not be opened.\n";
		//exit(2);
	}
}

//
// Detect and extract chessboard corners
//
void DuoCalibrator::detectChessboardPoints( const cv::Mat& left,
                                            const cv::Mat& right,
                                            std::vector<cv::Point2f>& leftPtsOut,
                                            std::vector<cv::Point2f>& rightPtsOut )
{
  static bool initObjectPts = true;

  if( initObjectPts )
  {
    //
    // 3D scene points:
    // Initialize the chessboard corners in the chessboard reference frame.
    // The corners are at location (x,y,z) = (i,j,0)
    //
    for( int i = 0; i < m_boardSize.height; ++i )
    {
      for( int j = 0; j < m_boardSize.width; ++j )
      {
        m_lastObjectPts.push_back( cv::Point3f( i*m_squareLength,
                                                j*m_squareLength,
                                                0.0f ) );
      }
    }

    initObjectPts = false;
  }

  const bool foundL = cv::findChessboardCorners( left,
                                                 m_boardSize,
                                                 leftPtsOut );

  const bool foundR = cv::findChessboardCorners( right,
                                                 m_boardSize,
                                                 rightPtsOut );

  //
  // Get sub-pixel accuracy on the corners
  //
  if( foundL && foundR )
  {
    static const auto termCrit =
        cv::TermCriteria( cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS,
                          20,    // max number of iterations
                          0.1 ); // min accuracy

    cv::cornerSubPix( left,
                      leftPtsOut,
                      cv::Size(  5,  5 ),
                      cv::Size( -1, -1 ),
                      termCrit );

    cv::cornerSubPix( right,
                      rightPtsOut,
                      cv::Size(  5,  5 ),
                      cv::Size( -1, -1 ),
                      termCrit );

    //
    // Board is good, save it
    //
    m_lastImagePtsL.assign( leftPtsOut.begin(),  leftPtsOut.end()  );
    m_lastImagePtsR.assign( rightPtsOut.begin(), rightPtsOut.end() );
  }
}


void DuoCalibrator::keepMostRecent()
{
  if( m_lastImagePtsL.size() == m_boardSize.area() &&
      m_lastImagePtsR.size() == m_boardSize.area() )
  {
    // 2D image points
    m_imagePtsL.push_back( m_lastImagePtsL );
    m_imagePtsR.push_back( m_lastImagePtsR );

    // 3D scene points
    m_objectPts.push_back( m_lastObjectPts );
  }
}


const cv::Mat& DuoCalibrator::undistortAndRectifyLeft( const cv::Mat& left ) const
{
  static cv::Mat newLeft;
  cv::remap( left, newLeft, m_mapL1, m_mapL2, cv::INTER_LINEAR );
  return newLeft;
}


const cv::Mat& DuoCalibrator::undistortAndRectifyRight( const cv::Mat& right ) const
{
  static cv::Mat newRight;
  cv::remap( right, newRight, m_mapR1, m_mapR2, cv::INTER_LINEAR );
  return newRight;
}


const cv::Mat& DuoCalibrator::getDisparity( const cv::Mat& left,
                                            const cv::Mat& right ) const
{
    static cv::Mat leftQVGA;
    static cv::Mat rightQVGA;
    
    cv::resize( left,  leftQVGA,  QVGA );
    cv::resize( right, rightQVGA, QVGA );
    
    static auto sgbm =
    //cv::StereoSGBM::create(   0,   //mindisp
    cv::StereoSGBM(   0,   //mindisp
                   48,   //numdisp
                   5,   //SADWindow
                   25,   //P1
                   50,   //P2
                   1,   //dispdiffmax
                   63,   //prefiltercap
                   40,   //uniqueness
                   200,   //speckle window size
                   2   //speckle range
                   );
    //         cv::StereoSGBM::MODE_SGBM_3WAY );
    
    static cv::Mat disp;
    sgbm( leftQVGA, rightQVGA, disp );
    
    static cv::Mat disp8;
    disp.convertTo( disp8, CV_8U, 255.0/(48*16) );
    
    static cv::Mat dispColor;
    cv::applyColorMap( disp8, dispColor, cv::COLORMAP_JET );
    
    return dispColor;
}

