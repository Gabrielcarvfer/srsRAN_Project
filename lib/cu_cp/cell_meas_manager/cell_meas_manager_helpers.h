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

#include "srsran/cu_cp/cell_meas_manager_config.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Perform sanity check on configuration struct.
bool is_valid_configuration(const cell_meas_manager_cfg& cfg);

/// \brief Log configured cells.
void log_cells(const srslog::basic_logger& logger, const cell_meas_manager_cfg& cfg);

} // namespace srs_cu_cp
} // namespace srsran
