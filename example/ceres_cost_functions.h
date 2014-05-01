#pragma once
#include <calibu/cam/camera_crtp.h>
#include <calibu/cam/camera_models_crtp.h>
#include <calibu/cam/camera_crtp_interop.h>
#include <sophus/se3.hpp>
#include <ceres/ceres.h>

/////////////////////////////////////////////////////////////////////////
template<typename CameraModel,typename Scalar=double>
struct ProjectionCostFunctor
{
  typedef Eigen::Matrix<Scalar, 2, 1> Vec2t;
  typedef Eigen::Matrix<Scalar, 3, 1> Vec3t;

  ProjectionCostFunctor(
      const Vec3t& _pwj, // 3D point j in the world
      const Vec2t& _zij,  // 2D image measurement of j from camrea i
      Scalar* _params
      ) : pwj(_pwj), zij(_zij), params(_params) 
  {
    //    t_vr = (calibu::RdfVision * calibu::RdfRobotics.inverse());
  }

  template<typename T>
    bool operator()(
        const T* const _t_wi,  // world pose of i'th camera
        T* residuals
        ) const 
    {
      CHECK_NOTNULL(_t_wi);
      CHECK_NOTNULL(residuals);
      const Eigen::Map<const Sophus::SE3Group<T> > t_wi(_t_wi);

      // get point j infront of camera i
      const Eigen::Matrix<T,3,1> pij = t_wi.inverse() * pwj.template cast<T>();
      T hij[2];
      CameraModel::template  Project<T>( pij.data(), (T*)params, hij );

      //      r = zij - hij;
      residuals[0] = zij(0)-hij[0];
      residuals[1] = zij(1)-hij[1];
      return true;
    }

  const Eigen::Matrix<Scalar,3,1>&  pwj; // point j in world frame
  const Eigen::Matrix<Scalar,2,1>&  zij; // measurement of i from frame j
  Scalar*                           params;
};


/////////////////////////////////////////////////////////////////////////
template<typename Scalar>
ceres::CostFunction* ProjectionCost(
    const Eigen::Vector3d& _pwj, // 3D point j in the world
    const Eigen::Vector2d& _zij, // 2D image measurement of j from camrea i
    calibu::CameraInterface<Scalar>* _cam
    )
{
  Scalar* _params = _cam->GetParams();
  if( dynamic_cast<calibu::LinearCamera<Scalar>*>( _cam ) ){
    typedef calibu::LinearCamera<Scalar> CamT;
    return (new ceres::AutoDiffCostFunction<ProjectionCostFunctor<CamT>,2,6>(
          new ProjectionCostFunctor<CamT>( _pwj,_zij,_params ) ) );
  }
  else if( dynamic_cast<calibu::FovCamera<Scalar>*>( _cam ) ){
    typedef calibu::FovCamera<Scalar> CamT;
    return (new ceres::AutoDiffCostFunction<ProjectionCostFunctor<CamT>,2,6>(
          new ProjectionCostFunctor<CamT>( _pwj,_zij,_params ) ) );
  }
  return NULL;
}

