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
    ResetArgs("-FLC");
    BOOST_CHECK(GetBoolArg("-FLC"));
    BOOST_CHECK(GetBoolArg("-FLC", false));
    BOOST_CHECK(GetBoolArg("-FLC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-FLCo"));
    BOOST_CHECK(!GetBoolArg("-FLCo", false));
    BOOST_CHECK(GetBoolArg("-FLCo", true));

    ResetArgs("-FLC=0");
    BOOST_CHECK(!GetBoolArg("-FLC"));
    BOOST_CHECK(!GetBoolArg("-FLC", false));
    BOOST_CHECK(!GetBoolArg("-FLC", true));

    ResetArgs("-FLC=1");
    BOOST_CHECK(GetBoolArg("-FLC"));
    BOOST_CHECK(GetBoolArg("-FLC", false));
    BOOST_CHECK(GetBoolArg("-FLC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noFLC");
    BOOST_CHECK(!GetBoolArg("-FLC"));
    BOOST_CHECK(!GetBoolArg("-FLC", false));
    BOOST_CHECK(!GetBoolArg("-FLC", true));

    ResetArgs("-noFLC=1");
    BOOST_CHECK(!GetBoolArg("-FLC"));
    BOOST_CHECK(!GetBoolArg("-FLC", false));
    BOOST_CHECK(!GetBoolArg("-FLC", true));

    ResetArgs("-FLC -noFLC");  // -FLC should win
    BOOST_CHECK(GetBoolArg("-FLC"));
    BOOST_CHECK(GetBoolArg("-FLC", false));
    BOOST_CHECK(GetBoolArg("-FLC", true));

    ResetArgs("-FLC=1 -noFLC=1");  // -FLC should win
    BOOST_CHECK(GetBoolArg("-FLC"));
    BOOST_CHECK(GetBoolArg("-FLC", false));
    BOOST_CHECK(GetBoolArg("-FLC", true));

    ResetArgs("-FLC=0 -noFLC=0");  // -FLC should win
    BOOST_CHECK(!GetBoolArg("-FLC"));
    BOOST_CHECK(!GetBoolArg("-FLC", false));
    BOOST_CHECK(!GetBoolArg("-FLC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--FLC=1");
    BOOST_CHECK(GetBoolArg("-FLC"));
    BOOST_CHECK(GetBoolArg("-FLC", false));
    BOOST_CHECK(GetBoolArg("-FLC", true));

    ResetArgs("--noFLC=1");
    BOOST_CHECK(!GetBoolArg("-FLC"));
    BOOST_CHECK(!GetBoolArg("-FLC", false));
    BOOST_CHECK(!GetBoolArg("-FLC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FLC", "eleven"), "eleven");

    ResetArgs("-FLC -bar");
    BOOST_CHECK_EQUAL(GetArg("-FLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FLC", "eleven"), "");

    ResetArgs("-FLC=");
    BOOST_CHECK_EQUAL(GetArg("-FLC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-FLC", "eleven"), "");

    ResetArgs("-FLC=11");
    BOOST_CHECK_EQUAL(GetArg("-FLC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-FLC", "eleven"), "11");

    ResetArgs("-FLC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-FLC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-FLC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-FLC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-FLC", 0), 0);

    ResetArgs("-FLC -bar");
    BOOST_CHECK_EQUAL(GetArg("-FLC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-FLC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-FLC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-FLC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-FLC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--FLC");
    BOOST_CHECK_EQUAL(GetBoolArg("-FLC"), true);

    ResetArgs("--FLC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-FLC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noFLC");
    BOOST_CHECK(!GetBoolArg("-FLC"));
    BOOST_CHECK(!GetBoolArg("-FLC", true));
    BOOST_CHECK(!GetBoolArg("-FLC", false));

    ResetArgs("-noFLC=1");
    BOOST_CHECK(!GetBoolArg("-FLC"));
    BOOST_CHECK(!GetBoolArg("-FLC", true));
    BOOST_CHECK(!GetBoolArg("-FLC", false));

    ResetArgs("-noFLC=0");
    BOOST_CHECK(GetBoolArg("-FLC"));
    BOOST_CHECK(GetBoolArg("-FLC", true));
    BOOST_CHECK(GetBoolArg("-FLC", false));

    ResetArgs("-FLC --noFLC");
    BOOST_CHECK(GetBoolArg("-FLC"));

    ResetArgs("-noFLC -FLC"); // FLC always wins:
    BOOST_CHECK(GetBoolArg("-FLC"));
}

BOOST_AUTO_TEST_SUITE_END()
