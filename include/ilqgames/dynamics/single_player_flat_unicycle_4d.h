/*
 * Copyright (c) 2019, The Regents of the University of California (Regents).
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *    2. Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *    3. Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS AS IS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Please contact the author(s) of this library if you have any questions.
 * Authors: David Fridovich-Keil   ( dfk@eecs.berkeley.edu )
 */

///////////////////////////////////////////////////////////////////////////////
//
// Single player dynamics modeling a unicycle. 4 states and 2 control inputs.
// State is [x, y, theta, v], control is [omega, a], and dynamics are:
//                     \dot px    = v cos theta
//                     \dot py    = v sin theta
//                     \dot theta = omega
//                     \dot v     = a
//
//  Linear system state xi is laid out as [x, y, vx, vy]:
//                     vx = v * cos(theta)
//                     vy = v * sin(theta)  
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ILQGAMES_DYNAMICS_SINGLE_PLAYER_FLAT_UNICYCLE_4D_H
#define ILQGAMES_DYNAMICS_SINGLE_PLAYER_FLAT_UNICYCLE_4D_H

#include <ilqgames/dynamics/single_player_flat_system.h>
#include <ilqgames/utils/types.h>

namespace ilqgames {

class SinglePlayerFlatUnicycle4D : public SinglePlayerFlatSystem {
 public:
  ~SinglePlayerFlatUnicycle4D() {}
  SinglePlayerFlatUnicycle4D()
      : SinglePlayerFlatSystem(kNumXDims, kNumUDims) {}

  // Compute time derivative of state.
  VectorXf Evaluate(const VectorXf& x, const VectorXf& u) const;

  // Discrete time approximation of the underlying linearized system.
  void LinearizeSystem(Time time_step,
                 Eigen::Ref<MatrixXf> A, Eigen::Ref<MatrixXf> B) const;

  // Utilities for feedback linearization.
  MatrixXf InverseDecouplingMatrix(const VectorXf& x) const;

  VectorXf AffineTerm(const VectorXf& x) const;

  VectorXf ToLinearSystemState(const VectorXf& x) const;

  VectorXf FromLinearSystemState(const VectorXf& xi) const;

  void GradientAndHessianXi(const VectorXf& xi, Eigen::Ref<VectorXf> grad,
                            Eigen::Ref<MatrixXf> hess)

  // Constexprs for state indices.
  static constexpr Dimension kNumXDims = 4;
  static constexpr Dimension kPxIdx = 0;
  static constexpr Dimension kPyIdx = 1;
  static constexpr Dimension kThetaIdx = 2;
  static constexpr Dimension kVIdx = 3;
  static constexpr Dimension kVxIdx = 2;
  static constexpr Dimension kVyIdx = 3;  

  // Constexprs for control indices.
  static constexpr Dimension kNumUDims = 2;
  static constexpr Dimension kOmegaIdx = 0;
  static constexpr Dimension kAIdx = 1;
};  //\class SinglePlayerFlatUnicycle4D

// ----------------------------- IMPLEMENTATION ----------------------------- //

inline VectorXf SinglePlayerFlatUnicycle4D::Evaluate(const VectorXf& x,
                                                 const VectorXf& u) const {
  VectorXf xdot(xdim_);
  xdot(kPxIdx) = x(kVIdx) * std::cos(x(kThetaIdx));
  xdot(kPyIdx) = x(kVIdx) * std::sin(x(kThetaIdx));
  xdot(kThetaIdx) = u(kOmegaIdx);
  xdot(kVIdx) = u(kAIdx);

  return xdot;
}

inline void SinglePlayerFlatUnicycle4D::LinearizedSystem(
                                              Time time_step,
                                              Eigen::Ref<MatrixXf> A,
                                              Eigen::Ref<MatrixXf> B) const {

  A(kPxIdx, kVxIdx) += time_step;
  A(kPyIdx, kVyIdx) += time_step;

  B(kVxIdx, 0) = time_step;
  B(kVyIdx, 1) = time_step;
}

inline MatrixXf InverseDecouplingMatrix(const VectorXf& x) const{
  MatrixXf M_inv(kNumUDims,kNumUDims);

  const float sin_t = std::sin(x(kThetaIdx));
  const float cos_t = std::cos(x(kThetaIdx));
   
  M_inv(0,0) = cos_t;
  M_inv(0,1) = sin_t;
  M_inv(1,0) = -sin_t / x(kVIdx);
  M_inv(1,1) =  cos_t / x(kVIdx);

  return M_inv;
}

inline VectorXf AffineTerm(const VectorXf& x) const{
  return VectorXf::Zero(kNumUDims);
}

inline VectorXf ToLinearSystemState(const VectorXf& x) const{
  VectorXf xi(kNumXDims);

  xi(kPxIdx) = x(kPxIdx);
  xi(kPyIdx) = x(kPyIdx);
  xi(kVxIdx) = x(kVIdx) * std::cos(x(kThetaIdx));
  xi(kVyIdx) = x(kVIdx) * std::sin(x(kThetaIdx));

  return xi;
}

inline VectorXf FromLinearSystemState(const VectorXf& xi) const{
  VectorXf x(kNumXDims);

  x(kPxIdx) = xi(kPxIdx);
  x(kPyIdx) = xi(kPyIdx);
  x(kThetaIdx) = std::atan2(xi(kVyIdx),xi(kVxIdx));
  x(kVIdx) = std::hypot(xi(kVyIdx),xi(kVxIdx));

  return x;
}

inline void GradientAndHessianXi(const VectorXf& xi, Eigen::Ref<VectorXf> grad,
                                    Eigen::Ref<MatrixXf> hess) const{

}

}  // namespace ilqgames
#endif
