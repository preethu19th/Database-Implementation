
#include "test.h"
#include "Comparison.h"
#include "gtest/gtest.h"

using namespace std;

class ComparisonTest : public ::testing::Test
{
protected:

    static void SetUpTestCase()
    {
	setup ();
    }
};

TEST_F  (ComparisonTest, order_maker_matching_sort_cols)
{
    OrderMaker sortorder, query1Order;
    n->get_sort_order (sortorder,"(n_name) AND (n_nationkey)");
    string query1 ("(n_nationkey = 0) AND (n_regionkey = 0) AND (n_name = '')");
    n->get_sort_order (query1Order,query1);

    EXPECT_EQ(true,sortorder.HasOrderedQueryCols(query1Order));
}
TEST_F  (ComparisonTest, order_maker_non_matching_sort_cols)
{
    OrderMaker sortorder, query2Order;
    n->get_sort_order (sortorder,"(n_name) AND (n_nationkey)");
    string query2 ("(n_nationkey > 0) AND (n_regionkey > 0) AND (n_name = '')");
    n->get_sort_order (query2Order,query2);

    EXPECT_EQ(false,sortorder.HasOrderedQueryCols(query2Order));
}
