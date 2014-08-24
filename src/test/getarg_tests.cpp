#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-BUY");
    BOOST_CHECK(GetBoolArg("-BUY"));
    BOOST_CHECK(GetBoolArg("-BUY", false));
    BOOST_CHECK(GetBoolArg("-BUY", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-BUYo"));
    BOOST_CHECK(!GetBoolArg("-BUYo", false));
    BOOST_CHECK(GetBoolArg("-BUYo", true));

    ResetArgs("-BUY=0");
    BOOST_CHECK(!GetBoolArg("-BUY"));
    BOOST_CHECK(!GetBoolArg("-BUY", false));
    BOOST_CHECK(!GetBoolArg("-BUY", true));

    ResetArgs("-BUY=1");
    BOOST_CHECK(GetBoolArg("-BUY"));
    BOOST_CHECK(GetBoolArg("-BUY", false));
    BOOST_CHECK(GetBoolArg("-BUY", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noBUY");
    BOOST_CHECK(!GetBoolArg("-BUY"));
    BOOST_CHECK(!GetBoolArg("-BUY", false));
    BOOST_CHECK(!GetBoolArg("-BUY", true));

    ResetArgs("-noBUY=1");
    BOOST_CHECK(!GetBoolArg("-BUY"));
    BOOST_CHECK(!GetBoolArg("-BUY", false));
    BOOST_CHECK(!GetBoolArg("-BUY", true));

    ResetArgs("-BUY -noBUY");  // -BUY should win
    BOOST_CHECK(GetBoolArg("-BUY"));
    BOOST_CHECK(GetBoolArg("-BUY", false));
    BOOST_CHECK(GetBoolArg("-BUY", true));

    ResetArgs("-BUY=1 -noBUY=1");  // -BUY should win
    BOOST_CHECK(GetBoolArg("-BUY"));
    BOOST_CHECK(GetBoolArg("-BUY", false));
    BOOST_CHECK(GetBoolArg("-BUY", true));

    ResetArgs("-BUY=0 -noBUY=0");  // -BUY should win
    BOOST_CHECK(!GetBoolArg("-BUY"));
    BOOST_CHECK(!GetBoolArg("-BUY", false));
    BOOST_CHECK(!GetBoolArg("-BUY", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--BUY=1");
    BOOST_CHECK(GetBoolArg("-BUY"));
    BOOST_CHECK(GetBoolArg("-BUY", false));
    BOOST_CHECK(GetBoolArg("-BUY", true));

    ResetArgs("--noBUY=1");
    BOOST_CHECK(!GetBoolArg("-BUY"));
    BOOST_CHECK(!GetBoolArg("-BUY", false));
    BOOST_CHECK(!GetBoolArg("-BUY", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BUY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BUY", "eleven"), "eleven");

    ResetArgs("-BUY -bar");
    BOOST_CHECK_EQUAL(GetArg("-BUY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BUY", "eleven"), "");

    ResetArgs("-BUY=");
    BOOST_CHECK_EQUAL(GetArg("-BUY", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-BUY", "eleven"), "");

    ResetArgs("-BUY=11");
    BOOST_CHECK_EQUAL(GetArg("-BUY", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-BUY", "eleven"), "11");

    ResetArgs("-BUY=eleven");
    BOOST_CHECK_EQUAL(GetArg("-BUY", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-BUY", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-BUY", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-BUY", 0), 0);

    ResetArgs("-BUY -bar");
    BOOST_CHECK_EQUAL(GetArg("-BUY", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-BUY=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-BUY", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-BUY=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-BUY", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--BUY");
    BOOST_CHECK_EQUAL(GetBoolArg("-BUY"), true);

    ResetArgs("--BUY=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-BUY", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noBUY");
    BOOST_CHECK(!GetBoolArg("-BUY"));
    BOOST_CHECK(!GetBoolArg("-BUY", true));
    BOOST_CHECK(!GetBoolArg("-BUY", false));

    ResetArgs("-noBUY=1");
    BOOST_CHECK(!GetBoolArg("-BUY"));
    BOOST_CHECK(!GetBoolArg("-BUY", true));
    BOOST_CHECK(!GetBoolArg("-BUY", false));

    ResetArgs("-noBUY=0");
    BOOST_CHECK(GetBoolArg("-BUY"));
    BOOST_CHECK(GetBoolArg("-BUY", true));
    BOOST_CHECK(GetBoolArg("-BUY", false));

    ResetArgs("-BUY --noBUY");
    BOOST_CHECK(GetBoolArg("-BUY"));

    ResetArgs("-noBUY -BUY"); // BUY always wins:
    BOOST_CHECK(GetBoolArg("-BUY"));
}

BOOST_AUTO_TEST_SUITE_END()
