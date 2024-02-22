/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/io/transport_layer_address.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

std::string create_random_ipv4_string()
{
  std::vector<uint8_t> nums = test_rgen::random_vector<uint8_t>(4);
  return fmt::format("{}.{}.{}.{}", nums[0], nums[1], nums[2], nums[3]);
}

std::string create_random_ipv6_string()
{
  std::vector<uint16_t> nums = test_rgen::random_vector<uint16_t>(8);
  return fmt::format("{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}:{:x}",
                     nums[0],
                     nums[1],
                     nums[2],
                     nums[3],
                     nums[4],
                     nums[5],
                     nums[6],
                     nums[7]);
}

TEST(transport_layer_address_test, empty_address)
{
  transport_layer_address addr{}, addr2{};
  ASSERT_EQ(fmt::format("{}", addr), "invalid_addr");
  ASSERT_EQ(addr, addr2);
}

TEST(transport_layer_address_test, conversion_to_ipv4_string)
{
  std::string ipv4_str = create_random_ipv4_string();
  auto        addr     = transport_layer_address::create_from_string(ipv4_str);
  ASSERT_EQ(addr.to_string(), ipv4_str);
  ASSERT_EQ(fmt::format("{}", addr), ipv4_str);
}

TEST(transport_layer_address_test, conversion_to_ipv6_string)
{
  std::string ipv6_str = create_random_ipv6_string();
  auto        addr     = transport_layer_address::create_from_string(ipv6_str);
  ASSERT_EQ(addr.to_string(), ipv6_str);
  ASSERT_EQ(fmt::format("{}", addr), ipv6_str);
}

TEST(transport_layer_address_test, ipv4_address_comparison)
{
  std::string ipv4_str1 = create_random_ipv4_string();
  std::string ipv4_str2 = create_random_ipv4_string();
  auto        addr1     = transport_layer_address::create_from_string(ipv4_str1);
  auto        addr2     = transport_layer_address::create_from_string(ipv4_str2);
  ASSERT_EQ(addr1, ipv4_str1);
  ASSERT_EQ(addr2, ipv4_str2);
}

TEST(transport_layer_address_test, ipv6_address_comparison)
{
  std::string ipv6_str1 = create_random_ipv6_string();
  std::string ipv6_str2 = create_random_ipv6_string();
  auto        addr1     = transport_layer_address::create_from_string(ipv6_str1);
  auto        addr2     = transport_layer_address::create_from_string(ipv6_str2);
  ASSERT_EQ(addr1, ipv6_str1);
  ASSERT_EQ(addr2, ipv6_str2);
}

TEST(transport_layer_address_test, ipv4_is_always_different_from_ipv6)
{
  std::string ipv4_str = create_random_ipv4_string();
  std::string ipv6_str = create_random_ipv6_string();
  auto        addr1    = transport_layer_address::create_from_string(ipv4_str);
  auto        addr2    = transport_layer_address::create_from_string(ipv6_str);
  ASSERT_NE(addr1, addr2);
}
