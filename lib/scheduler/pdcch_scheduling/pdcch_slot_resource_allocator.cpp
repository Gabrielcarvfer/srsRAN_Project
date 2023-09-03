/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_slot_resource_allocator.h"
#include "../cell/resource_grid.h"
#include "pdcch_config_helpers.h"
#include "srsran/ran/pdcch/cce_to_prb_mapping.h"

using namespace srsran;

pdcch_slot_allocator::~pdcch_slot_allocator() {}

void pdcch_slot_allocator::clear()
{
  records.clear();
  dfs_tree.clear();
  saved_dfs_tree.clear();
}

bool pdcch_slot_allocator::alloc_pdcch(dci_context_information&          pdcch_ctx,
                                       cell_slot_resource_allocator&     slot_alloc,
                                       const search_space_configuration& ss_cfg,
                                       const pdcch_candidate_list&       ss_candidates)
{
  saved_dfs_tree.clear();

  // Create an DL Allocation Record.
  alloc_record record{};
  record.is_dl            = true;
  record.pdcch_ctx        = &pdcch_ctx;
  record.ss_cfg           = &ss_cfg;
  record.pdcch_candidates = ss_candidates;

  // Try to allocate PDCCH for one of the possible CCE positions. If this operation fails, retry it, but using a
  // different permutation of past grant CCE positions.
  do {
    if (alloc_dfs_node(slot_alloc, record, 0)) {
      // Set CCE start index.
      record.pdcch_ctx->cces.ncce = dfs_tree.back().ncce;
      // DCI record was successfully allocated.
      records.push_back(record);
      // If the previous branches of the DFS changed in order for this new PDCCH to be allocated, update previous
      // PDCCH ncces
      if (not saved_dfs_tree.empty()) {
        for (unsigned i = 0; i != dfs_tree.size() - 1; ++i) {
          records[dfs_tree[i].record_index].pdcch_ctx->cces.ncce = dfs_tree[i].ncce;
        }
      }
      return true;
    }
    if (saved_dfs_tree.empty()) {
      saved_dfs_tree = dfs_tree;
    }
  } while (get_next_dfs(slot_alloc));

  // Revert steps to initial state, before dci record allocation was attempted
  dfs_tree.swap(saved_dfs_tree);

  return false;
}

bool pdcch_slot_allocator::cancel_last_pdcch(cell_slot_resource_allocator& slot_alloc)
{
  if (records.empty()) {
    return false;
  }
  if (not slot_alloc.result.dl.dl_pdcchs.empty() and
      records.back().pdcch_ctx == &slot_alloc.result.dl.dl_pdcchs.back().ctx) {
    slot_alloc.result.dl.dl_pdcchs.pop_back();
  } else if (not slot_alloc.result.dl.ul_pdcchs.empty() and
             records.back().pdcch_ctx == &slot_alloc.result.dl.ul_pdcchs.back().ctx) {
    slot_alloc.result.dl.ul_pdcchs.pop_back();
  } else {
    return false;
  }

  dfs_tree.pop_back();
  records.pop_back();
  return true;
}

bool pdcch_slot_allocator::alloc_dfs_node(cell_slot_resource_allocator& slot_alloc,
                                          const alloc_record&           record,
                                          unsigned                      dci_iter_index)
{
  // Get CCE location table, i.e. the current node possible leaves.
  span<const uint8_t> cce_locs = record.pdcch_candidates;
  if (dci_iter_index >= cce_locs.size()) {
    // All possible CCE position leaves have been attempted. Early return.
    return false;
  }

  // Create new tree leave.
  tree_node node{};
  node.dci_iter_index = dci_iter_index;
  node.record_index   = dfs_tree.size();

  // Find in the list of possible CCEs, a CCE that does not cause PDCCH collisions.
  for (; node.dci_iter_index < cce_locs.size(); ++node.dci_iter_index) {
    node.ncce = cce_locs[node.dci_iter_index];

    // Attempt to allocate PDCCH with provided CCE position.
    if (not allocate_cce(slot_alloc, node.ncce, record)) {
      continue;
    }

    // Allocation successful. Add node to tree.
    dfs_tree.push_back(node);
    return true;
  }
  return false;
}

bool pdcch_slot_allocator::get_next_dfs(cell_slot_resource_allocator& slot_alloc)
{
  do {
    if (dfs_tree.empty()) {
      // If we reach root, the allocation failed.
      return false;
    }
    // Attempt to re-add last tree node, but with a higher leaf index.
    uint32_t start_child_idx = dfs_tree.back().dci_iter_index + 1;
    dfs_tree.pop_back();
    while (dfs_tree.size() < records.size() and alloc_dfs_node(slot_alloc, records[dfs_tree.size()], start_child_idx)) {
      start_child_idx = 0;
    }
  } while (dfs_tree.size() < records.size());

  // Finished computation of next DFS node.
  return true;
}

bool pdcch_slot_allocator::allocate_cce(cell_slot_resource_allocator& slot_alloc,
                                        unsigned                      ncce,
                                        const alloc_record&           record)
{
  const bwp_configuration&     bwp_cfg = *record.pdcch_ctx->bwp_cfg;
  const coreset_configuration& cs_cfg  = *record.pdcch_ctx->coreset_cfg;
  prb_index_list               pdcch_prbs =
      pdcch_helper::cce_to_prb_mapping(bwp_cfg, cs_cfg, cell_cfg.pci, record.pdcch_ctx->cces.aggr_lvl, ncce);
  grant_info grant;
  grant.scs = bwp_cfg.scs;

  // Check the current CCE position collides with an existing one.
  // TODO: Optimize.
  for (uint16_t prb : pdcch_prbs) {
    unsigned crb  = prb_to_crb(*record.pdcch_ctx->bwp_cfg, prb);
    grant.crbs    = {crb, crb + 1};
    grant.symbols = {0, (uint8_t)cs_cfg.duration};
    if (slot_alloc.dl_res_grid.collides(grant)) {
      // Collision detected. Try another CCE position.
      return false;
    }
  }

  // Allocation successful.
  // TODO: Optimize.
  for (uint16_t prb : pdcch_prbs) {
    unsigned crb  = prb_to_crb(*record.pdcch_ctx->bwp_cfg, prb);
    grant.crbs    = {crb, crb + 1};
    grant.symbols = {0, (uint8_t)cs_cfg.duration};
    slot_alloc.dl_res_grid.fill(grant);
  }

  return true;
}
