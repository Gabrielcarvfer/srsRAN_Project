/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"

namespace srsgnb {
namespace srs_cu_cp {

/// @brief  Convert CU-CP security result to NGAP security result.
/// @param cu_cp_security_result The CU-CP security result.
/// @return The NGAP security result.
inline asn1::ngap::security_result_s
cu_cp_security_result_to_ngap_security_result(cu_cp_security_result cu_cp_security_result)
{
  asn1::ngap::security_result_s ngap_security_result;

  if (cu_cp_security_result.confidentiality_protection_result == "performed") {
    ngap_security_result.confidentiality_protection_result =
        asn1::ngap::confidentiality_protection_result_opts::options::performed;
  } else {
    ngap_security_result.confidentiality_protection_result =
        asn1::ngap::confidentiality_protection_result_opts::options::not_performed;
  }

  if (cu_cp_security_result.integrity_protection_result == "performed") {
    ngap_security_result.integrity_protection_result.value =
        asn1::ngap::integrity_protection_result_opts::options::performed;
  } else {
    ngap_security_result.integrity_protection_result =
        asn1::ngap::integrity_protection_result_opts::options::not_performed;
  }

  return ngap_security_result;
}

/// @brief Convert CU-CP Associated QoS Flow to NGAP Associated QoS Flow Item.
/// @param cu_cp_qos_flow The CU-CP Associated QoS Flow.
/// @return The NGAP Associated QoS Flow Item.
inline asn1::ngap::associated_qos_flow_item_s
cu_cp_assoc_qos_flow_to_ngap_assoc_qos_flow_item(cu_cp_associated_qos_flow cu_cp_qos_flow)
{
  asn1::ngap::associated_qos_flow_item_s asn1_assoc_qos_item;

  asn1_assoc_qos_item.qos_flow_id = qos_flow_id_to_uint(cu_cp_qos_flow.qos_flow_id);

  if (cu_cp_qos_flow.qos_flow_map_ind.has_value()) {
    asn1_assoc_qos_item.qos_flow_map_ind_present = true;

    if (cu_cp_qos_flow.qos_flow_map_ind.value() == "ul") {
      asn1_assoc_qos_item.qos_flow_map_ind.value =
          asn1::ngap::associated_qos_flow_item_s::qos_flow_map_ind_opts::options::ul;
    } else {
      asn1_assoc_qos_item.qos_flow_map_ind.value =
          asn1::ngap::associated_qos_flow_item_s::qos_flow_map_ind_opts::options::dl;
    }
  }

  return asn1_assoc_qos_item;
}

/// @brief Convert CU-CP QoS Flow Per TNL Info to NGAP QoS Flow Per TNL Info.
/// @param cu_cp_qos_flow_info The CU-CP QoS Flow Per TNL Info.
/// @return The NGAP QoS Flow Per TNL Info.
inline asn1::ngap::qos_flow_per_tnl_info_s
cu_cp_qos_flow_per_tnl_info_to_ngap_qos_flow_per_tnl_info(cu_cp_qos_flow_per_tnl_information cu_cp_qos_flow_info)
{
  asn1::ngap::qos_flow_per_tnl_info_s ngap_qos_flow_info;

  up_transport_layer_info_to_asn1(ngap_qos_flow_info.up_transport_layer_info, cu_cp_qos_flow_info.up_tp_layer_info);

  for (const auto& cu_cp_assoc_qos_item_pair : cu_cp_qos_flow_info.associated_qos_flow_list) {
    const auto&                            cu_cp_assoc_qos_item = cu_cp_assoc_qos_item_pair.second;
    asn1::ngap::associated_qos_flow_item_s ngap_assoc_qos_item =
        cu_cp_assoc_qos_flow_to_ngap_assoc_qos_flow_item(cu_cp_assoc_qos_item);

    ngap_qos_flow_info.associated_qos_flow_list.push_back(ngap_assoc_qos_item);
  }

  return ngap_qos_flow_info;
}

/// @brief Convert CU-CP cause to NGAP cause.
/// @param cu_cp_cause The CU-CP cause.
/// @return The NGAP cause.
inline asn1::ngap::cause_c cu_cp_cause_to_ngap_cause(cu_cp_cause_t cu_cp_cause)
{
  asn1::ngap::cause_c ngap_cause;

  switch (cu_cp_cause) {
    case cu_cp_cause_t::radio_network:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::radio_network);
      return ngap_cause;
      break;
    case cu_cp_cause_t::transport:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::transport);
      return ngap_cause;
      break;
    case cu_cp_cause_t::protocol:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::protocol);
      return ngap_cause;
      break;
    case cu_cp_cause_t::misc:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::misc);
      return ngap_cause;
      break;
    default:
      ngap_cause.set(asn1::ngap::cause_c::types_opts::nulltype);
      return ngap_cause;
  }
}

/// @brief Convert CU-CP QoS Flow Failed to Setup Item to NGAP QoS Flow With Cause Item.
/// @param cu_cp_failed_item The CU-CP QoS Flow Failed to Setup Item.
/// @return The NGAP QoS Flow With Cause Item.
inline asn1::ngap::qos_flow_with_cause_item_s cu_cp_qos_flow_failed_to_setup_item_to_ngap_qos_flow_with_cause_item(
    cu_cp_qos_flow_failed_to_setup_item cu_cp_failed_item)
{
  asn1::ngap::qos_flow_with_cause_item_s asn1_failed_item;
  asn1_failed_item.qos_flow_id = qos_flow_id_to_uint(cu_cp_failed_item.qos_flow_id);
  asn1_failed_item.cause       = cu_cp_cause_to_ngap_cause(cu_cp_failed_item.cause);

  return asn1_failed_item;
}

} // namespace srs_cu_cp
} // namespace srsgnb
