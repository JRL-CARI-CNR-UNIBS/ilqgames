/*
 * Copyright (c) 2020, The Regents of the University of California (Regents).
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
// Utility for solving a problem using a receding horizon, simulating dynamics
// forward at each stage to account for the passage of time and also using
// defensive driving strategy for the ego but normal strategies for others.
//
// This class is intended as a facsimile of a real-time, online receding horizon
// problem in which short horizon problems are solved asynchronously throughout
// operation.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef ILQGAMES_EXAMPLES_DEFENSIVE_DRIVING_RECEDING_HORIZON_SIMULATOR_H
#define ILQGAMES_EXAMPLES_DEFENSIVE_DRIVING_RECEDING_HORIZON_SIMULATOR_H

#include <ilqgames/solver/game_solver.h>
#include <ilqgames/solver/solution_splicer.h>
#include <ilqgames/utils/solver_log.h>

#include <memory>
#include <vector>

namespace ilqgames {

// Solve this game following a receding horizon with strategies for all players
// drawn from defensive or normal problems as above.
void DefensiveDrivingRecedingHorizonSimulator(
    Time final_time, Time planner_runtime, GameSolver *defensive,
    GameSolver *normal,
    std::vector<std::shared_ptr<const SolverLog>> *defensive_logs,
    std::vector<std::shared_ptr<const SolverLog>> *normal_logs);

} // namespace ilqgames

#endif
