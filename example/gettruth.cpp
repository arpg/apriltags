#include <iostream>
#include <vector>

#include <glog/logging.h>
#include <calibu/cam/CameraXml.h>
#include <calibu/cam/CameraModelT.h>

#include <calibu/cam/camera_crtp.h>
#include <calibu/cam/camera_models_crtp.h>
#include <calibu/cam/camera_crtp_interop.h>

#include <sophus/se3.hpp>
#include <ceres/ceres.h>

#include "ceres_cost_functions.h"

namespace Eigen
{
  typedef Matrix<double,6,1>  Vector6d;
}

using namespace std;
using namespace ceres;
using namespace Eigen;


/////////////////////////////////////////////////////////////////////////
class LocalizationProblem 
{
  public:
    int num_observations() const 
    {
      return num_observations_;               
    }

    const Eigen::Vector6d& pose_for_observation( int i ) 
    {
      return poses_[pose_index_[i]];
    }

    double* pose_data_for_observation( int i ) 
    {
      return poses_[pose_index_[i]].data();
    }

    const Eigen::Vector3d& landmark_for_observation(int i) {
      return landmarks_[landmark_index_[i]];
    }

    const Eigen::Vector2d& observation(int i) {
      return observations_[i];
    }

    bool LoadFile( const char* filename )
    {
      FILE* fptr = fopen(filename, "r");
      if (fptr == NULL) {
        return false;
      };

      FscanfOrDie( fptr, "%d", &num_poses_ );
      FscanfOrDie( fptr, "%d", &num_landmarks_ );
      FscanfOrDie( fptr, "%d", &num_observations_ );

      landmark_index_.resize( num_observations_ );
      pose_index_.resize( num_observations_ );
      observations_.resize( num_observations_ );
      poses_.resize( num_poses_ );
      landmarks_.resize( num_landmarks_ );

      num_parameters_ = 6 * num_poses_; // just a localization problem

      for (int i = 0; i < num_observations_; ++i) {
        FscanfOrDie( fptr, "%d", &pose_index_[i] );
        FscanfOrDie( fptr, "%d", &landmark_index_[i] );

        double u, v;
        FscanfOrDie(fptr, "%lf", &u );
        FscanfOrDie(fptr, "%lf", &v );
        observations_[i] = Eigen::Vector2d(u,v);
      }

      // poses
      for (int i = 0; i < num_poses_; ++i) {
        double x,y,z,p,q,r;
        int n = fscanf( fptr, "%lf, %lf, %lf, %lf, %lf, %lf", 
            &x, &y , &z , &p , &q , &r );
        if( n != 6 ){
          LOG(FATAL) << "Error reading pose " << i << ".";
        }
        poses_[i] << x, y, z, p, q, r;
      }

      // landmarks
      for (int i = 0; i < num_landmarks_; ++i) {
        double x,y,z;
        int n = fscanf( fptr, "%lf, %lf, %lf", &x, &y , &z );
        if( n != 3 ){
          LOG(FATAL) << "Error reading landmark " << i << ".";
        }
        landmarks_[i] << x, y, z;
      }

      return true;
    }

  private:
    template<typename T>
      void FscanfOrDie(FILE *fptr, const char *format, T *value) {
        int num_scanned = fscanf(fptr, format, value);
        if (num_scanned != 1) {
          LOG(FATAL) << "Invalid UW data file.";
        }
      }

    int num_poses_;
    int num_landmarks_;
    int num_observations_;
    int num_parameters_;

    std::vector<int> landmark_index_;
    std::vector<int> pose_index_;
    std::vector<Eigen::Vector6d> poses_; // parameters
    std::vector<Eigen::Vector3d> landmarks_;
    std::vector<Eigen::Vector2d> observations_;
};


/////////////////////////////////////////////////////////////////////////
int main( int argc, char** argv )
{
  google::InitGoogleLogging(argv[0]);

  if (argc != 2) {
    std::cerr << "usage: gettruth <measurement_file>\n";
    return 1;
  }

  // get camera model
  calibu::Rig<double> rig;
  calibu::LoadRig( "cmod.xml", &rig );
  calibu::CameraInterface<double>* cam = rig.cameras_[0];

  LocalizationProblem ctx;
  if (!ctx.LoadFile(argv[1])) {
    std::cerr << "ERROR: unable to open file " << argv[1] << "\n";
    return 1;
  }

  // Build the problem.
  Problem problem;

  for (int i = 0; i < ctx.num_observations(); ++i) {
    const Vector2d& measurement = ctx.observation(i);
    const Vector3d& landmark = ctx.landmark_for_observation(i);
    ceres::CostFunction* cost_function = ProjectionCost( landmark, measurement, cam );
    problem.AddResidualBlock( cost_function, NULL, ctx.pose_data_for_observation(i) );
  }

  // Make Ceres automatically detect the bundle structure. Note that the
  // standard solver, SPARSE_NORMAL_CHOLESKY, also works fine but it is slower
  // for standard bundle adjustment problems.
  ceres::Solver::Options options;
  options.linear_solver_type = ceres::DENSE_SCHUR;
  options.minimizer_progress_to_stdout = true;

  ceres::Solver::Summary summary;
  ceres::Solve(options, &problem, &summary);
  std::cout << summary.FullReport() << "\n";

  return 0;
}
