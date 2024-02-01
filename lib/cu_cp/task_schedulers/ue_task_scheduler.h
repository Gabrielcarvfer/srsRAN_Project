/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/fifo_async_task_scheduler.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/timers.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// \brief Service provided by CU-CP to schedule async tasks for a given UE.
class ue_task_scheduler
{
public:
  explicit ue_task_scheduler(timer_manager&        timers_,
                             task_executor&        exec_,
                             unsigned              max_nof_supported_ues,
                             srslog::basic_logger& logger_);
  ~ue_task_scheduler() = default;

  // UE task scheduler
  void handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task);

  void clear_pending_tasks(ue_index_t ue_index);

  async_task<bool> dispatch_and_await_task_completion(ue_index_t ue_index, unique_task task);

  unique_timer   make_unique_timer();
  timer_manager& get_timer_manager();

private:
  timer_manager&        timers;
  task_executor&        exec;
  srslog::basic_logger& logger;

  // task event loops indexed by ue_index
  std::unordered_map<ue_index_t, fifo_async_task_scheduler> ue_ctrl_loop;
};

} // namespace srs_cu_cp
} // namespace srsran
