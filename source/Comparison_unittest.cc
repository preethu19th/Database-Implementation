
#include "test.h"
#include "Comparison.h"
#include "gtest/gtest.h"

using namespace std;

class ComparisonTest : public ::testing::Test
{
protected:
    static CNF cnf;
    static Record literal;
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
    static void VerifyLiteralOrder (int numAtts,string query) {
        n->get_cnf (cnf, literal, query);
        EXPECT_EQ (numAtts, cnf.HasOrderedQueryCols (sortorder, queryOrder, literalOrder));
        for (int i = 0; i < queryOrder.numAtts; i++) {
            EXPECT_EQ (queryOrder.whichTypes[i], literalOrder.whichTypes[i]);
        }
    }
};

CNF ComparisonTest :: cnf;
Record ComparisonTest :: literal;
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
    VerifyLiteralOrder(1, query1);
}

TEST_F (ComparisonTest, order_maker_matching_sort_cols_out_of_order)
{
    VerifyLiteralOrder(1, query2);
}

TEST_F (ComparisonTest, order_maker_matching_sort_cols_in_order)
{
    VerifyLiteralOrder(1, query3);
}

TEST_F (ComparisonTest, order_maker_matching_first_sort_cols_1)
{
    VerifyLiteralOrder(1,query4);
}

TEST_F (ComparisonTest, order_maker_matching_2nd_sort_col)
{
    VerifyLiteralOrder(0,query5);
}

TEST_F (ComparisonTest, order_maker_matching_first_sort_cols_2)
{
    VerifyLiteralOrder(1,query6);
}

TEST_F (ComparisonTest, order_maker_non_matching_sort_cols)
{
    VerifyLiteralOrder(0,query7);
}
