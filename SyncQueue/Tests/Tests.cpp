#include <gtest/gtest.h>
#include <queue>
#include <stack>
#include <list>
#include <set>
#include "../Optional.h"
#include "../Detector.h"

TEST(Detector, HaveMethods) {
    // hasPush
    ASSERT_TRUE(Detector<std::queue<Optional<int> > >::hasPush);
    ASSERT_TRUE(Detector<std::stack<Optional<int> > >::hasPush);
    ASSERT_FALSE(Detector<std::vector<Optional<int> > >::hasPush);
    ASSERT_FALSE(Detector<std::list<Optional<int> > >::hasPush);
    ASSERT_FALSE(Detector<std::deque<Optional<int> > >::hasPush);

    // hasPop
    ASSERT_TRUE(Detector<std::queue<Optional<int> > >::hasPop);
    ASSERT_TRUE(Detector<std::stack<Optional<int> > >::hasPop);
    ASSERT_FALSE(Detector<std::vector<Optional<int> > >::hasPop);
    ASSERT_FALSE(Detector<std::list<Optional<int> > >::hasPop);
    ASSERT_FALSE(Detector<std::deque<Optional<int> > >::hasPop);

    // hasPushBack
    ASSERT_FALSE(Detector<std::queue<Optional<int> > >::hasPushBack);
    ASSERT_FALSE(Detector<std::stack<Optional<int> > >::hasPushBack);
    ASSERT_TRUE(Detector<std::vector<Optional<int> > >::hasPushBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPushBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPushBack);

    // hasPopBack
    ASSERT_FALSE(Detector<std::queue<Optional<int> > >::hasPopBack);
    ASSERT_FALSE(Detector<std::stack<Optional<int> > >::hasPopBack);
    ASSERT_TRUE(Detector<std::vector<Optional<int> > >::hasPopBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPopBack);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::hasPopBack);
}

TEST(Detector, Suitability) {
    // push() and pop()
    ASSERT_TRUE(Detector<std::queue<Optional<int> > >::pushPopSuitable);
    ASSERT_TRUE(Detector<std::stack<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::vector<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::list<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::deque<Optional<int> > >::pushPopSuitable);

    // push_back() and pop_back()
    ASSERT_FALSE(Detector<std::queue<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_FALSE(Detector<std::stack<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_TRUE(Detector<std::vector<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_TRUE(Detector<std::list<Optional<int> > >::pushBackPopBackSuitable);
    ASSERT_TRUE(Detector<std::deque<Optional<int> > >::pushBackPopBackSuitable);
}

TEST(Detector, Unsuitability) {
    ASSERT_FALSE(Detector<std::set<Optional<int> > >::pushPopSuitable);
    ASSERT_FALSE(Detector<std::set<Optional<int> > >::pushBackPopBackSuitable);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}