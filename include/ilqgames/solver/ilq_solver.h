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
// Base class for all iterative LQ game solvers. Derives from FeedbackSolver.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ILQGAMES_SOLVER_ILQ_SOLVER_H
#define ILQGAMES_SOLVER_ILQ_SOLVER_H

#include <ilqgames/cost/player_cost.h>
#include <ilqgames/dynamics/multi_player_dynamical_system.h>
#include <ilqgames/solver/game_solver.h>
#include <ilqgames/solver/lq_feedback_solver.h>
#include <ilqgames/solver/lq_solver.h>
#include <ilqgames/solver/solver_params.h>
#include <ilqgames/utils/linear_dynamics_approximation.h>
#include <ilqgames/utils/operating_point.h>
#include <ilqgames/utils/quadratic_cost_approximation.h>
#include <ilqgames/utils/solver_log.h>
#include <ilqgames/utils/strategy.h>
#include <ilqgames/utils/types.h>

#include <glog/logging.h>
#include <limits>
#include <memory>
#include <vector>

namespace ilqgames {

class ILQSolver : public GameSolver<LinearDynamicsApproximation,
                                    QuadraticCostApproximation> {
 public:
  virtual ~ILQSolver() {}
  ILQSolver(const std::shared_ptr<Problem>& problem,
            const SolverParams& params = SolverParams())
      : GameSolver(problem, params) {
    // Set up LQ solver.
    if (params_.open_loop)
      lq_solver_.reset(
          new LQOpenLoopSolver(problem_->Dynamics(), problem_->NumTimeSteps()));
    else
      lq_solver_.reset(
          new LQFeedbackSolver(problem_->Dynamics(), problem_->NumTimeSteps()));

    // Prepopulate quadraticization.
    for (auto& quads : quadraticization_)
      quads.resize(problem_->Dynamics()->NumPlayers(),
                   QuadraticCostApproximation(problem_->Dynamics()->XDim()));
  }

  // Solve this game. Returns true if converged.
  virtual std::shared_ptr<SolverLog> Solve(
      bool* success = nullptr,
      Time max_runtime = std::numeric_limits<Time>::infinity());

 protected:
  // Modify LQ strategies to improve convergence properties.
  // This function replaces an Armijo linesearch that would take place in ILQR.
  // Returns true if successful, and records if we have converged and the total
  // costs for all players at the new operating point, as well as the times at
  // which each player achieves an extreme cost.
  virtual bool ModifyLQStrategies(std::vector<Strategy>* strategies,
                                  OperatingPoint* current_operating_point,
                                  bool* is_new_operating_point_feasible,
                                  bool* has_converged,
                                  std::vector<float>* total_costs) const;

  // Compute distance (infinity norm) between states in the given dimensions.
  // If dimensions empty, checks all dimensions.
  virtual float StateDistance(const VectorXf& x1, const VectorXf& x2,
                              const std::vector<Dimension>& dims) const;

  // Compute the current operating point based on the current set of strategies
  // and the last operating point. Checks whether the solver has converged and
  // populates the total costs for all players of the new operating point.
  // Returns true if the new operating point satisfies the trust region
  // (including all explicit inequality constraints), or if the
  // `check_trust_region` flag is false. Optionally also returns the times of
  // extreme costs.
  virtual bool CurrentOperatingPoint(
      const OperatingPoint& last_operating_point,
      const std::vector<Strategy>& current_strategies,
      OperatingPoint* current_operating_point, bool* has_converged,
      std::vector<float>* total_costs, bool check_trust_region = true,
      bool* satisfies_constraints = nullptr) const;

  // Core LQ Solver.
  std::unique_ptr<LQSolver> lq_solver_;
};  // class ILQSolver

}  // namespace ilqgames

#endif
