/*
 * IFVRMonoCameraCalib.cpp
 *
 *  Created on: 2017年3月7日
 *      Author: clover
 */

#include "IFVRMonoCameraCalib.h"

enum { DETECTION = 0, CAPTURING = 1, CALIBRATED = 2 };
enum Pattern { CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };

using namespace std;
static double computeReprojectionErrors(
        const vector<vector<cv::Point3f> >& objectPoints,
        const vector<vector<cv::Point2f> >& imagePoints,
        const vector<cv::Mat>& rvecs, const vector<cv::Mat>& tvecs,
        const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
        vector<float>& perViewErrors )
{
    vector<cv::Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); i++ )
    {
        projectPoints(cv::Mat(objectPoints[i]), rvecs[i], tvecs[i],
                      cameraMatrix, distCoeffs, imagePoints2);
        err = norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), CV_L2);
        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float)std::sqrt(err*err/n);
        totalErr += err*err;
        totalPoints += n;
    }

    return std::sqrt(totalErr/totalPoints);
}

static void calcChessboardCorners(cv::Size boardSize, float squareSize, vector<cv::Point3f>& corners, Pattern patternType = CHESSBOARD)
{
    corners.resize(0);

    switch(patternType)
    {
      case CHESSBOARD:
      case CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(cv::Point3f(float(j*squareSize),
                                          float(i*squareSize), 0));
        break;

      case ASYMMETRIC_CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(cv::Point3f(float((2*j + i % 2)*squareSize),
                                          float(i*squareSize), 0));
        break;

      default:
        CV_Error(CV_StsBadArg, "Unknown pattern type\n");
    }
}

static bool runCalibration( vector<vector<cv::Point2f> > imagePoints,
                    cv::Size imageSize, cv::Size boardSize, Pattern patternType,
                    float squareSize, float aspectRatio,
                    int flags, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
                    vector<cv::Mat>& rvecs, vector<cv::Mat>& tvecs,
                    vector<float>& reprojErrs,
                    double& totalAvgErr)
{
    cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    if( flags & CV_CALIB_FIX_ASPECT_RATIO )
        cameraMatrix.at<double>(0,0) = aspectRatio;

    distCoeffs = cv::Mat::zeros(8, 1, CV_64F);

    vector<vector<cv::Point3f> > objectPoints(1);
    calcChessboardCorners(boardSize, squareSize, objectPoints[0], patternType);

    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    double rms = calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                    distCoeffs, rvecs, tvecs, flags|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
                    ///*|CV_CALIB_FIX_K3*/|CV_CALIB_FIX_K4|CV_CALIB_FIX_K5);
    printf("RMS error reported by calibrateCamera: %g\n", rms);

    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}


static void saveCameraParams( const string& filename,
                       cv::Size imageSize, cv::Size boardSize,
                       float squareSize, float aspectRatio, int flags,
                       const cv::Mat& cameraMatrix, const cv::Mat& distCoeffs,
                       const vector<cv::Mat>& rvecs, const vector<cv::Mat>& tvecs,
                       const vector<float>& reprojErrs,
                       const vector<vector<cv::Point2f> >& imagePoints,
                       double totalAvgErr )
{
    cv::FileStorage fs( filename, cv::FileStorage::WRITE );

    time_t tt;
    time( &tt );
    struct tm *t2 = localtime( &tt );
    char buf[1024];
    strftime( buf, sizeof(buf)-1, "%c", t2 );

    fs << "calibration_time" << buf;

    if( !rvecs.empty() || !reprojErrs.empty() )
        fs << "nframes" << (int)std::max(rvecs.size(), reprojErrs.size());
    fs << "image_width" << imageSize.width;
    fs << "image_height" << imageSize.height;
    fs << "board_width" << boardSize.width;
    fs << "board_height" << boardSize.height;
    fs << "square_size" << squareSize;

    if( flags & CV_CALIB_FIX_ASPECT_RATIO )
        fs << "aspectRatio" << aspectRatio;

    if( flags != 0 )
    {
        sprintf( buf, "flags: %s%s%s%s",
            flags & CV_CALIB_USE_INTRINSIC_GUESS ? "+use_intrinsic_guess" : "",
            flags & CV_CALIB_FIX_ASPECT_RATIO ? "+fix_aspectRatio" : "",
            flags & CV_CALIB_FIX_PRINCIPAL_POINT ? "+fix_principal_point" : "",
            flags & CV_CALIB_ZERO_TANGENT_DIST ? "+zero_tangent_dist" : "" );
        cvWriteComment( *fs, buf, 0 );
    }

    fs << "flags" << flags;

    fs << "camera_matrix" << cameraMatrix;
    fs << "distortion_coefficients" << distCoeffs;

    fs << "avg_reprojection_error" << totalAvgErr;
    if( !reprojErrs.empty() )
        fs << "per_view_reprojection_errors" << cv::Mat(reprojErrs);

    if( !rvecs.empty() && !tvecs.empty() )
    {
        CV_Assert(rvecs[0].type() == tvecs[0].type());
        cv::Mat bigmat((int)rvecs.size(), 6, rvecs[0].type());
        for( int i = 0; i < (int)rvecs.size(); i++ )
        {
            cv::Mat r = bigmat(cv::Range(i, i+1), cv::Range(0,3));
            cv::Mat t = bigmat(cv::Range(i, i+1), cv::Range(3,6));

            CV_Assert(rvecs[i].rows == 3 && rvecs[i].cols == 1);
            CV_Assert(tvecs[i].rows == 3 && tvecs[i].cols == 1);
            //*.t() is MatExpr (not Mat) so we can use assignment operator
            r = rvecs[i].t();
            t = tvecs[i].t();
        }
        cvWriteComment( *fs, "a set of 6-tuples (rotation vector + translation vector) for each view", 0 );
        fs << "extrinsic_parameters" << bigmat;
    }

    if( !imagePoints.empty() )
    {
        cv::Mat imagePtMat((int)imagePoints.size(), (int)imagePoints[0].size(), CV_32FC2);
        for( int i = 0; i < (int)imagePoints.size(); i++ )
        {
            cv::Mat r = imagePtMat.row(i).reshape(2, imagePtMat.cols);
            cv::Mat imgpti(imagePoints[i]);
            imgpti.copyTo(r);
        }
        fs << "image_points" << imagePtMat;
    }
}

static bool readStringList( const string& filename, vector<string>& l )
{
    l.resize(0);
    cv::FileStorage fs(filename, cv::FileStorage::READ);
    if( !fs.isOpened() )
        return false;
    cv::FileNode n = fs.getFirstTopLevelNode();
    if( n.type() != cv::FileNode::SEQ )
        return false;
    cv::FileNodeIterator it = n.begin(), it_end = n.end();
    for( ; it != it_end; ++it )
        l.push_back((string)*it);
    return true;
}


static bool runAndSave(const string& outputFilename,
                const vector<vector<cv::Point2f> >& imagePoints,
                cv::Size imageSize, cv::Size boardSize, Pattern patternType, float squareSize,
                float aspectRatio, int flags, cv::Mat& cameraMatrix,
                cv::Mat& distCoeffs, bool writeExtrinsics, bool writePoints )
{
    vector<cv::Mat> rvecs, tvecs;
    vector<float> reprojErrs;
    double totalAvgErr = 0;

    bool ok = runCalibration(imagePoints, imageSize, boardSize, patternType, squareSize,
                   aspectRatio, flags, cameraMatrix, distCoeffs,
                   rvecs, tvecs, reprojErrs, totalAvgErr);
    printf("%s. avg reprojection error = %.2f\n",
           ok ? "Calibration succeeded" : "Calibration failed",
           totalAvgErr);

    /*
    if( ok )
        saveCameraParams( outputFilename, imageSize,
                         boardSize, squareSize, aspectRatio,
                         flags, cameraMatrix, distCoeffs,
                         writeExtrinsics ? rvecs : vector<cv::Mat>(),
                         writeExtrinsics ? tvecs : vector<cv::Mat>(),
                         writeExtrinsics ? reprojErrs : vector<float>(),
                         writePoints ? imagePoints : vector<vector<cv::Point2f> >(),
                         totalAvgErr );
    */
    return ok;
}

IFVRMonoCameraCalib::IFVRMonoCameraCalib(cv::Size chessSize) {
	// TODO Auto-generated constructor stub
	m_chessboardSize = chessSize;
	m_imagePoints.clear();
	m_imageSize = cv::Size(752,480);
}

IFVRMonoCameraCalib::~IFVRMonoCameraCalib() {
	// TODO Auto-generated destructor stub
}

bool IFVRMonoCameraCalib::saveCurrentPoint(){
	if(m_oneFramePoints.size() != 0){
		m_imagePoints.push_back(m_oneFramePoints);
		printf("m_oneFramePoints saved\n");
		return true;
	}else{
		printf("m_oneFramePoints is empty\n");
	}
	return false;
}

//若采集成功返回ture
bool IFVRMonoCameraCalib::captureMat(cv::Mat inputMat, bool isDrawChess, cv::Mat& drawChessMat)
{
	//vector<cv::Point2f> pointbuf;
	m_oneFramePoints.clear();
	drawChessMat = inputMat.clone();
	//std::cout<<drawChessMat.channels() << drawChessMat.depth() << std::endl;
	if(drawChessMat.channels() == 1)
	{
		cv::cvtColor(drawChessMat, drawChessMat, CV_GRAY2BGR);
	}

	bool found = cv::findChessboardCorners(inputMat, m_chessboardSize, m_oneFramePoints,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);
	if(found)
		cv::drawChessboardCorners( drawChessMat, m_chessboardSize, cv::Mat(m_oneFramePoints), found );

	return found;
}
//返回矫正误差
bool IFVRMonoCameraCalib::runCalibAndSaveTo(std::string calibFilePath)
{
	bool ok = false;
	if(m_imagePoints.size() > 8){
		ok = runAndSave(calibFilePath,m_imagePoints,m_imageSize, m_chessboardSize, CHESSBOARD,0.019,1.0,0, m_cameraMatrix, m_distCoeffs, false, false);
		if(ok)
			initUndistortRectifyMap(m_cameraMatrix, m_distCoeffs, cv::Mat(),
					cv::getOptimalNewCameraMatrix(m_cameraMatrix, m_distCoeffs, m_imageSize, 1, m_imageSize, 0),
					m_imageSize, CV_16SC2, m_map1, m_map2);
		else
			printf( "runAndSave errr \n");

		m_imagePoints.clear();
		}else{
		printf("m_imagePoints less\n");
	}
	return ok;
}
void IFVRMonoCameraCalib::runCheckCalib(const cv::Mat&orgMat, cv::Mat& checkMat)
{
	remap(orgMat, checkMat, m_map1, m_map2, CV_INTER_LINEAR);
}
