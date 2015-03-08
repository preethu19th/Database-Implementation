
#include "test.h"
#include "Comparison.h"
#include "gtest/gtest.h"

using namespace std;

class ComparisonTest : public ::testing::Test
{
protected:
    static OrderMaker sortorder, queryOrder;
    static string sorder, query1, query2, query3, query4, query5, query6;
    static void SetUpTestCase()
    {
        setup ();
        n->get_sort_order (sortorder, sorder);
    }
};

OrderMaker ComparisonTest :: sortorder, ComparisonTest :: queryOrder;
string ComparisonTest :: sorder ("(n_name) AND (n_nationkey)");
string ComparisonTest :: query1 ("(n_nationkey = 0) AND (n_regionkey = 0) AND (n_name = '')");
string ComparisonTest :: query2 ("(n_nationkey = 0) AND (n_name = '')");
string ComparisonTest :: query3 ("(n_name = '') AND (n_nationkey = 0)");
string ComparisonTest :: query4 ("(n_name = '')");
string ComparisonTest :: query5 ("(n_nationkey = 0)");
string ComparisonTest :: query6 ("(n_nationkey > 0) AND (n_regionkey > 0) AND (n_name = '')");

TEST_F (ComparisonTest, order_maker_matching_sort_cols_w_extra_cols)
{
    n->get_sort_order (queryOrder,query1);
    EXPECT_EQ(true,sortorder.HasOrderedQueryCols(queryOrder));
}

TEST_F (ComparisonTest, order_maker_matching_sort_cols_out_of_order)
{
    n->get_sort_order (queryOrder,query2);
    EXPECT_EQ(true,sortorder.HasOrderedQueryCols(queryOrder));
}

TEST_F (ComparisonTest, order_maker_matching_sort_cols_in_order)
{
    n->get_sort_order (queryOrder,query3);
    EXPECT_EQ(true,sortorder.HasOrderedQueryCols(queryOrder));
}

TEST_F (ComparisonTest, order_maker_non_matching_sort_cols_only_single_col_1)
{
    n->get_sort_order (queryOrder,query4);
    EXPECT_EQ(false,sortorder.HasOrderedQueryCols(queryOrder));
}

TEST_F (ComparisonTest, order_maker_non_matching_sort_cols_only_single_col_2)
{
    n->get_sort_order (queryOrder,query5);
    EXPECT_EQ(false,sortorder.HasOrderedQueryCols(queryOrder));
}

TEST_F (ComparisonTest, order_maker_non_matching_sort_cols)
{
    n->get_sort_order (queryOrder,query6);
    EXPECT_EQ(false,sortorder.HasOrderedQueryCols(queryOrder));
}
