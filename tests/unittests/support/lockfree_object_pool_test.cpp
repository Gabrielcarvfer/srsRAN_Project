/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/memory_pool/lockfree_object_pool.h"
#include <gtest/gtest.h>

using namespace srsran;

TEST(lockfree_object_pool_test, pool_created_with_requested_capacity)
{
  unsigned                  nof_objs = 10;
  int                       val      = 3;
  lockfree_object_pool<int> pool(nof_objs, val);

  ASSERT_EQ(pool.capacity(), nof_objs);
  ASSERT_EQ(pool.estimated_size(), pool.capacity());
}

TEST(lockfree_object_pool_test, pool_allocates_elements_with_initialized_value)
{
  unsigned nof_objs = 10;
  int      val      = 5;

  lockfree_object_pool<int> pool(nof_objs, val);

  std::vector<lockfree_object_pool<int>::ptr> allocated_objs;
  for (unsigned i = 0; i != nof_objs; ++i) {
    ASSERT_EQ(pool.estimated_size(), pool.capacity() - i);
    allocated_objs.push_back(pool.get());
    ASSERT_EQ(*allocated_objs.back(), val);
  }
  ASSERT_EQ(pool.estimated_size(), 0);
  ASSERT_EQ(pool.get(), nullptr);

  allocated_objs.clear();
  ASSERT_EQ(pool.estimated_size(), nof_objs);

  auto ptr = pool.get();
  ASSERT_EQ(*ptr, val);
  ASSERT_EQ(pool.estimated_size(), nof_objs - 1);
}

TEST(lockfree_bounded_stack_test, test_initialization)
{
  lockfree_bounded_stack<int> stack{10};
  ASSERT_EQ(stack.capacity(), 10);
  ASSERT_EQ(stack.size(), 0);
}

TEST(lockfree_bounded_stack_test, test_push_pop_one_element)
{
  lockfree_bounded_stack<int> stack{10};

  stack.push(5);
  ASSERT_EQ(stack.size(), 1);
  ASSERT_EQ(stack.capacity(), 10);

  int val;
  ASSERT_TRUE(stack.pop(val));
  ASSERT_EQ(val, 5);
  ASSERT_EQ(stack.size(), 0);
  ASSERT_EQ(stack.capacity(), 10);
}

TEST(lockfree_bounded_stack_test, test_pop_empty)
{
  lockfree_bounded_stack<int> stack{10};

  int val = 3;
  ASSERT_FALSE(stack.pop(val));
  stack.push(val);
  ASSERT_TRUE(stack.pop(val));
  ASSERT_EQ(val, 3);
  ASSERT_FALSE(stack.pop(val));
}
