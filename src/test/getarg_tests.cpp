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
    ResetArgs("-LVC");
    BOOST_CHECK(GetBoolArg("-LVC"));
    BOOST_CHECK(GetBoolArg("-LVC", false));
    BOOST_CHECK(GetBoolArg("-LVC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-LVCo"));
    BOOST_CHECK(!GetBoolArg("-LVCo", false));
    BOOST_CHECK(GetBoolArg("-LVCo", true));

    ResetArgs("-LVC=0");
    BOOST_CHECK(!GetBoolArg("-LVC"));
    BOOST_CHECK(!GetBoolArg("-LVC", false));
    BOOST_CHECK(!GetBoolArg("-LVC", true));

    ResetArgs("-LVC=1");
    BOOST_CHECK(GetBoolArg("-LVC"));
    BOOST_CHECK(GetBoolArg("-LVC", false));
    BOOST_CHECK(GetBoolArg("-LVC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noLVC");
    BOOST_CHECK(!GetBoolArg("-LVC"));
    BOOST_CHECK(!GetBoolArg("-LVC", false));
    BOOST_CHECK(!GetBoolArg("-LVC", true));

    ResetArgs("-noLVC=1");
    BOOST_CHECK(!GetBoolArg("-LVC"));
    BOOST_CHECK(!GetBoolArg("-LVC", false));
    BOOST_CHECK(!GetBoolArg("-LVC", true));

    ResetArgs("-LVC -noLVC");  // -LVC should win
    BOOST_CHECK(GetBoolArg("-LVC"));
    BOOST_CHECK(GetBoolArg("-LVC", false));
    BOOST_CHECK(GetBoolArg("-LVC", true));

    ResetArgs("-LVC=1 -noLVC=1");  // -LVC should win
    BOOST_CHECK(GetBoolArg("-LVC"));
    BOOST_CHECK(GetBoolArg("-LVC", false));
    BOOST_CHECK(GetBoolArg("-LVC", true));

    ResetArgs("-LVC=0 -noLVC=0");  // -LVC should win
    BOOST_CHECK(!GetBoolArg("-LVC"));
    BOOST_CHECK(!GetBoolArg("-LVC", false));
    BOOST_CHECK(!GetBoolArg("-LVC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--LVC=1");
    BOOST_CHECK(GetBoolArg("-LVC"));
    BOOST_CHECK(GetBoolArg("-LVC", false));
    BOOST_CHECK(GetBoolArg("-LVC", true));

    ResetArgs("--noLVC=1");
    BOOST_CHECK(!GetBoolArg("-LVC"));
    BOOST_CHECK(!GetBoolArg("-LVC", false));
    BOOST_CHECK(!GetBoolArg("-LVC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-LVC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LVC", "eleven"), "eleven");

    ResetArgs("-LVC -bar");
    BOOST_CHECK_EQUAL(GetArg("-LVC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LVC", "eleven"), "");

    ResetArgs("-LVC=");
    BOOST_CHECK_EQUAL(GetArg("-LVC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-LVC", "eleven"), "");

    ResetArgs("-LVC=11");
    BOOST_CHECK_EQUAL(GetArg("-LVC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-LVC", "eleven"), "11");

    ResetArgs("-LVC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-LVC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-LVC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-LVC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-LVC", 0), 0);

    ResetArgs("-LVC -bar");
    BOOST_CHECK_EQUAL(GetArg("-LVC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-LVC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-LVC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-LVC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-LVC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--LVC");
    BOOST_CHECK_EQUAL(GetBoolArg("-LVC"), true);

    ResetArgs("--LVC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-LVC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noLVC");
    BOOST_CHECK(!GetBoolArg("-LVC"));
    BOOST_CHECK(!GetBoolArg("-LVC", true));
    BOOST_CHECK(!GetBoolArg("-LVC", false));

    ResetArgs("-noLVC=1");
    BOOST_CHECK(!GetBoolArg("-LVC"));
    BOOST_CHECK(!GetBoolArg("-LVC", true));
    BOOST_CHECK(!GetBoolArg("-LVC", false));

    ResetArgs("-noLVC=0");
    BOOST_CHECK(GetBoolArg("-LVC"));
    BOOST_CHECK(GetBoolArg("-LVC", true));
    BOOST_CHECK(GetBoolArg("-LVC", false));

    ResetArgs("-LVC --noLVC");
    BOOST_CHECK(GetBoolArg("-LVC"));

    ResetArgs("-noLVC -LVC"); // LVC always wins:
    BOOST_CHECK(GetBoolArg("-LVC"));
}

BOOST_AUTO_TEST_SUITE_END()
