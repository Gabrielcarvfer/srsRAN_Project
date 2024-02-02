/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dummy_du.h"
#include "srsran/adt/concurrent_queue.h"
#include "srsran/cu_cp/du_repository.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace srs_cu_cp;

namespace {

class manual_mock_du final : public du_test_stub
{
public:
  manual_mock_du(du_stub_params params) : cu_cp_f1c(params.cu_cp_f1c_itf)
  {
    tx_pdu_notifier = cu_cp_f1c.handle_new_du_connection(std::make_unique<rx_pdu_notifier>(*this));
  }

  bool connected() const { return tx_pdu_notifier != nullptr; }

  void push_tx_pdu(const f1ap_message& msg) override
  {
    report_fatal_error_if_not(tx_pdu_notifier != nullptr, "TNL connection is not established");
    tx_pdu_notifier->on_new_message(msg);
  }

  bool try_pop_rx_pdu(f1ap_message& msg) override { return rx_pdus.try_pop(msg); }

private:
  class rx_pdu_notifier final : public f1ap_message_notifier
  {
  public:
    rx_pdu_notifier(manual_mock_du& parent_) : parent(parent_) {}

    void on_new_message(const f1ap_message& msg) override { parent.handle_rx_pdu(msg); }

  private:
    manual_mock_du& parent;
  };

  void handle_rx_pdu(const f1ap_message& msg)
  {
    report_fatal_error_if_not(rx_pdus.push_blocking(msg), ");
  }

  du_repository&                         cu_cp_f1c;
  std::unique_ptr<f1ap_message_notifier> tx_pdu_notifier;

  concurrent_queue<f1ap_message,
                   srsran::concurrent_queue_policy::locking_mpmc,
                   srsran::concurrent_queue_wait_policy::condition_variable>
      rx_pdus{1024};
};

} // namespace

std::unique_ptr<du_test_stub> srsran::srs_cu_cp::create_du_stub(du_stub_params params)
{
  auto du = std::make_unique<manual_mock_du>(params);
  if (not du->connected()) {
    return nullptr;
  }
  return du;
}
