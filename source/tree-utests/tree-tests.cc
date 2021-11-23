
#include <algorithm>
#include <gtest/gtest.h>
#include <random>
#include <vector>

#include "tree-tester.hh"

namespace tree
{

namespace
{
constexpr size_t TEST_INSERTS_NUM = 1000;
constexpr size_t TEST_ERASES_NUM = TEST_INSERTS_NUM / 5;

using testData = std::vector<size_t>;

void shuffle(testData data)
{
  static auto rEng = std::default_random_engine{};
  std::shuffle(std::begin(data), std::end(data), rEng);
}

testData genShuffled(size_t size)
{
  std::vector<size_t> toRet(size);
  std::iota(std::begin(toRet), std::end(toRet), 0);
  shuffle(toRet);
  return toRet;
}

} // namespace

TEST(StatTreeTests, InsertTest)
{
  auto toInsert = genShuffled(TEST_INSERTS_NUM);

  TreeTester::TestTree tree{};
  for (size_t i = 0; i < TEST_INSERTS_NUM; ++i)
    tree.insert(toInsert[i]);

  for (size_t i = 0; i < TEST_INSERTS_NUM; ++i)
    ASSERT_FALSE(tree.find(toInsert[i]) == tree.end());

  ASSERT_FALSE(TreeTester::verify(tree));
}

TEST(StatTreeTests, DeleteTest)
{
  auto toInsert = genShuffled(TEST_INSERTS_NUM);

  TreeTester::TestTree tree{};
  for (size_t i = 0; i < TEST_INSERTS_NUM; ++i)
    tree.insert(toInsert[i]);

  // Deleting data in all possible range.
  auto toErase = genShuffled(TEST_INSERTS_NUM);
  for (size_t i = 0; i < TEST_ERASES_NUM; ++i)
    tree.erase(tree.find(toErase[i]));

  for (size_t i = 0; i < TEST_ERASES_NUM; ++i)
    ASSERT_TRUE(tree.find(toErase[i]) == tree.end());

  for (size_t i = TEST_ERASES_NUM; i < TEST_INSERTS_NUM; ++i)
    ASSERT_FALSE(tree.find(toErase[i]) == tree.end());

  ASSERT_FALSE(TreeTester::verify(tree));
}

} // namespace tree
