
#include "test.h"
#include "Comparison.h"
#include "gtest/gtest.h"

using namespace std;

class ComparisonTest : public ::testing::Test
{
protected:
    static OrderMaker sortorder, queryOrder, literalOrder;
    static string sorder, query1, query2, query3, query4, query5, query6, query7;
    static void SetUpTestCase ()
    {
        setup ();
        n->get_sort_order (sortorder, sorder);
    }
    static void TearDownTestCase () {
        cleanup ();
    }
    static void VerifyLiteralOrder (int numAtts) {
        EXPECT_EQ (numAtts, sortorder.HasOrderedQueryCols (queryOrder, literalOrder));
        EXPECT_EQ (numAtts, literalOrder.numAtts);
        for (int i = 0; i < numAtts; i++) {
            EXPECT_EQ (sortorder.whichAtts[i], literalOrder.whichAtts[i]);
            EXPECT_EQ (sortorder.whichTypes[i], literalOrder.whichTypes[i]);
        }
    }
};

OrderMaker ComparisonTest :: sortorder, ComparisonTest :: queryOrder;
OrderMaker ComparisonTest :: literalOrder;
string ComparisonTest :: sorder ("(n_name) AND (n_nationkey)");
string ComparisonTest :: query1 ("(n_nationkey = 0) AND (n_regionkey = 0) AND (n_name = '')");
string ComparisonTest :: query2 ("(n_nationkey = 0) AND (n_name = '')");
string ComparisonTest :: query3 ("(n_name = '') AND (n_nationkey = 0)");
string ComparisonTest :: query4 ("(n_name = '')");
string ComparisonTest :: query5 ("(n_nationkey = 0)");
string ComparisonTest :: query6 ("(n_nationkey > 0) AND (n_regionkey > 0) AND (n_name = '')");
string ComparisonTest :: query7 ("(n_nationkey > 0) AND (n_regionkey > 0) AND (n_name > '')");

TEST_F (ComparisonTest, order_maker_matching_sort_cols_w_extra_cols)
{
    n->get_sort_order (queryOrder, query1);
    VerifyLiteralOrder(2);
}

TEST_F (ComparisonTest, order_maker_matching_sort_cols_out_of_order)
{
    n->get_sort_order (queryOrder, query2);
    VerifyLiteralOrder(2);
}

TEST_F (ComparisonTest, order_maker_matching_sort_cols_in_order)
{
    n->get_sort_order (queryOrder, query3);
    VerifyLiteralOrder(2);
}

TEST_F (ComparisonTest, order_maker_matching_first_sort_cols_1)
{
    n->get_sort_order (queryOrder, query4);
    VerifyLiteralOrder(1);
}

TEST_F (ComparisonTest, order_maker_matching_2nd_sort_col)
{
    n->get_sort_order (queryOrder, query5);
    VerifyLiteralOrder(0);
}

TEST_F (ComparisonTest, order_maker_matching_first_sort_cols_2)
{
    n->get_sort_order (queryOrder, query6);
    VerifyLiteralOrder(1);
}

TEST_F (ComparisonTest, order_maker_non_matching_sort_cols)
{
    n->get_sort_order (queryOrder, query7);
    VerifyLiteralOrder(0);
}
