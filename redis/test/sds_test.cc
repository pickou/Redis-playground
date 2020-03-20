#include <iostream>
#include <math.h>
#include "thirdparty/gtest/gtest.h"
#include "redis/src/sds.h"


class SdsTest: public ::testing::Test {
    protected:
        SdsTest() {

        }

        ~SdsTest() override {}

        void SetUp() override {}

        void TearDown() override {}
};

testing::AssertionResult INFO_Assert_EQ(const char* m_expr, const char* n_expr, int m, int n) {
    if(m == n) {
        return testing::AssertionSuccess();
    }

    testing::Message msg;

    msg << m_expr << " and " << n_expr << " expected: " << m << " actual: " << n << "\n";
    return testing::AssertionFailure(msg);
}

TEST_F(SdsTest, Sdsnew) {
    char p[] = "adsfsfs";
    sds s = sdsnew(p);
    ASSERT_NE(s, nullptr);
    EXPECT_EQ(sdslen(s), 7);
    
    char q[] = "tgbye";
    sds ss = sdsnew(q);
    ASSERT_NE(ss, nullptr);
    EXPECT_EQ(sdslen(ss), 5);
    
    int lenss = sdslen(ss);
    int lens = sdslen(s);

    int minlen = lens > lenss ? lenss : lens;

    for(int i=0; i< minlen; ++i) {
        ASSERT_NE(s[i], ss[i]);
    }

    ASSERT_NE(s, ss);

    sdsfree(s);
    sdsfree(ss);

    char a[] = "abc";
    char b[] = "abcd";

    sds sa = sdsnew(a);
    sds sb = sdsnew(b);

    sdsfree(sa);
    sdsfree(sb);
}

TEST_F(SdsTest, Sdsavail) {
    char p[] = "abde";
    sds s = sdsnew(p);
    EXPECT_EQ(0, sdsavail(s));
    sdsfree(s);
}

TEST_F(SdsTest, Sdsfree) {
    char p[] = "adsbd";
    sds s = sdsnew(p);
    sdsfree(s);
    // TODO: how to set pointer to NULL in all scope
    // EXPECT_TRUE(s == NULL);  
    // EXPECT_EQ(s, nullptr);
}

TEST_F(SdsTest, Sdscat ) {
    char p[] = "strafasf";
    char exp_str[] = "sdsstrafasf";
    char q[] = "sds";
    sds sdst = sdsnew(q);
    sds s = sdscat(sdst, p);
    EXPECT_EQ(sdslen(s), 11);
    for(int i=0; i<sdslen(s); ++i) {
         EXPECT_EQ(s[i], exp_str[i]);
     }
    sdsfree(s);
    // TODO: how to avoid sdst free,
    // this cause double free
}

TEST_F(SdsTest, Sdsupperlower) {
    char p[] = "AbcDEfg";
    char lower_str[] = "abcdefg";
    char upper_str[] = "ABCDEFG";

    sds lower_sds = sdsnew(p);
    ASSERT_EQ(sdslen(lower_sds), strlen(p));
    sdstolower(lower_sds);
    for(int i=0; i<sdslen(lower_sds); ++i) {
        EXPECT_EQ(lower_str[i], lower_str[i]);
    }
    
    sdsfree(lower_sds);

    sds upper_sds = sdsnew(p);
    ASSERT_EQ(sdslen(upper_sds), strlen(p));
    sdstoupper(upper_sds);
    for(int i=0; i<sdslen(upper_sds); ++i) {
        EXPECT_EQ(upper_sds[i], upper_str[i]);
    }
    
    sdsfree(upper_sds);
}

TEST_F(SdsTest, Sdscmp) {
    char p1[] = "abca";
    sds s1 = sdsnew(p1);
    
    char p2[] = "asdf";
    sds s2 = sdsnew(p2);
    
    ASSERT_FALSE(s1 == NULL);
    EXPECT_EQ(sdslen(s1), 4);
    ASSERT_FALSE(s2 == NULL);
    EXPECT_EQ(sdslen(s2), 4);
   
    int cmp = sdscmp(s2, s1);

    EXPECT_TRUE(cmp > 0);

    char p3[] = "abcae";
    sds s3 = sdsnew(p3);

    cmp = sdscmp(s1, s3);
    // try the detailed info test
    EXPECT_PRED_FORMAT2(INFO_Assert_EQ, -1, cmp);

    sdsfree(s3);
    sdsfree(s2);
    sdsfree(s1);

}

TEST_F(SdsTest, Sdscpy) {
    char p[] = "12345";
    char q[] = "abcd";
    sds s = sdsnew(p);
    ASSERT_FALSE(s == NULL);
    s = sdscpy(s, q);
    ASSERT_EQ(4, sdslen(s));
    for(int i=0; i<sdslen(s); ++i) {
        EXPECT_EQ(s[i], q[i]);
    }
    
    EXPECT_EQ(1, sdsavail(s));

    char r[] = "123456";
    s = sdscpy(s, r);
    ASSERT_EQ(6, sdslen(s));
    for(int i=0; i<6; ++i) {
        EXPECT_EQ(s[i], r[i]);
    }

    sdsfree(s);
}
