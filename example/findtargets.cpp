#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <vector>

#include <gflags/gflags.h>
#include <glog/logging.h>

#include <calibu/cam/camera_crtp.h>
#include <calibu/cam/camera_models_crtp.h>
//#include <calibu/cam/camera_crtp_interop.h>
#include <calibu/cam/camera_crtp_impl.h>

#include <HAL/Camera/CameraDevice.h>

#include "tags.h"

namespace Eigen
{
  typedef Matrix<double,6,1>  Vector6d;
}

using namespace std;

DEFINE_string(cam,
    "-cam",
    "hal camera specifier");

/////////////////////////////////////////////////////////////////////////
void ParseCameraUriOrDieComplaining( const std::string& sUri, hal::Camera& cam )
{
  try{
    cam = hal::Camera( hal::Uri(sUri) );
  }
  catch( hal::DeviceException e ){
    printf("Error parsing camera URI: '%s' -- %s\n", sUri.c_str(), e.what() );
    printf("Perhaps you meant something like one of these:\n" );
    printf("    rectify:[file=cameras.xml]//deinterlace://uvc://\n" );
    printf("    file:[loop=1]//~/Data/CityBlock-Noisy/[left*,right*].pgm\n" );
    exit(-1);
  }
}

/////////////////////////////////////////////////////////////////////////
image_u8* ImageU8( const cv::Mat& im )
{
  static image_u8 tmp;
  tmp.width = im.cols;
  tmp.height = im.rows;
  tmp.buf = im.data;
  return &tmp;
}

/////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  hal::Camera cam;
  ParseCameraUriOrDieComplaining( FLAGS_cam, cam );

  // get a tag detector
  //  AprilTags::TagCodes tagCodes( AprilTags::tagCodes36h11 );
  //  AprilTags::TagDetector* tagDetector = new AprilTags::TagDetector( tagCodes ); 

  /*
  //td->seg_sigma = atof(argv[++i]);
  april_tag_family_t*     tf_;
  april_tag_detector_t*   td_;
  april_tag_detector_destroy( td_ );
  tag36h11_destroy( tf_ );
  */

//  TagDetector td;
  april_tag_family_t*    tf_ = tag36h11_create();
  april_tag_detector_t*  td_ = april_tag_detector_create(tf_);
  td_->debug = 1;
  td_->small_tag_refinement = 1;

  cv::namedWindow( "Tag Viewer", CV_WINDOW_AUTOSIZE );

  std::vector<cv::Mat> vImages;
  for( int ii = 0; true; ii++ ){
    if( !cam.Capture( vImages ) ){
      printf("Finished after image %d\n", ii );
      return 0;
    }

    cv::imshow( "Tag Viewer", vImages[0] ); 
    printf("read image %d\n", ii );

//    zarray_t *detections =
//      april_tag_detector_detect( td_, ImageU8(vImages[0]) );

//             april_tag_detector_detect(td, im);
//    tagDetector->extractTags( vImages[0] );

      /*
         1) extract target correspondencs, 3D landmarks: apriltags c++ code
         2) compute PNP global pose: calibu?
         3) for each "landmark" record camera index, landmark point index, u,v
       */
    cv::waitKey(0);
  }
 
  // Save observation info to a file as:
  // number of poses
  // number of landmarks
  // number of observations
  // cam id, pt id, u, v
  // cam id, pt id, u, v
  // ...
  // cam id, pt id, u, v
  // 6xm vector for the pose of the m cameras
  // 3xm vector for the n global landmark positions

  return 0;
}
