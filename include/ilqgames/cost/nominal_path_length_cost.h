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
// Quadratic penalty on deviation from a nominal path length, which depends upon
// time and desired speed.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ILQGAMES_COST_NOMINAL_PATH_LENGTH_H
#define ILQGAMES_COST_NOMINAL_PATH_LENGTH_H

#include <ilqgames/cost/cost.h>
#include <ilqgames/utils/types.h>

#include <string>

namespace ilqgames {

class NominalPathLengthCost : public Cost {
 public:
  // Construct from a multiplicative weight and the dimension in which to apply
  // the quadratic cost (difference from nominal).
  NominalPathLengthCost(float weight, Dimension dim, float nominal_speed,
                        const std::string& name = "")
      : Cost(weight, name), dimension_(dim), nominal_speed_(nominal_speed) {}

  // Evaluate this cost at the current time and input.
  float Evaluate(Time t, const VectorXf& input) const;

  // Quadraticize this cost at the given time and input, and add to the running
  // sum of gradients and Hessians.
  void Quadraticize(Time t, const VectorXf& input, MatrixXf* hess,
                    VectorXf* grad) const;

 private:
  // Dimension in which to apply the quadratic cost.
  const Dimension dimension_;

  // Nominal speed.
  const float nominal_speed_;
};  //\class NominalPathLengthCost

}  // namespace ilqgames

#endif
