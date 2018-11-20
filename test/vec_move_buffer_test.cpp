// vec_test.cpp
//

#include <vector>
#include <memory>

#include "dtm/vec.hpp"

#include "catch.hpp"
#include "construction_test_type.hpp"

TEST_CASE("vec_move_buffer_construction", "[vec]") {
    SECTION("default_vec_is_empty") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec;
        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 0);
        CHECK(vec.empty());
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("N_default_construction_calls") {
        construction_test_type::reset();
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(5);
        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 5);
        CHECK(!vec.empty());
        CHECK(construction_test_type::num_default_constructions == 5);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("N_copy_construction_calls") {
        construction_test_type::reset();
        construction_test_type construction_test;
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(5, construction_test);
        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 5);
        CHECK(!vec.empty());
        CHECK(construction_test_type::num_default_constructions == 1);
        CHECK(construction_test_type::num_copy_constructions == 5);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("N_default_construction_values") {
        dtm::vec<int,dtm::detail::move_buffer<int>> vec(2);
        CHECK(vec.size() == 2);
        CHECK(vec.capacity() == 2);
        CHECK(!vec.empty());
        CHECK(vec[0] == 0);
        CHECK(vec[1] == 0);
    }

    SECTION("N_copy_construction_calls") {
        std::vector<int> v(2, 1);

        int val = 1;
        dtm::vec<int,dtm::detail::move_buffer<int>> vec(2, val);
        CHECK(vec.size() == 2);
        CHECK(vec.capacity() == 2);
        CHECK(!vec.empty());
        CHECK(vec[0] == 1);
        CHECK(vec[1] == 1);
    }

    SECTION("copy_construction_values") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v1(2, 1);
        v1.reserve(5);

        dtm::vec<int,dtm::detail::move_buffer<int>> v2(v1);
        CHECK(v2.size() == 2);
        CHECK(v2.capacity() == 2);
        CHECK(v2[0] == 1);
        CHECK(v2[1] == 1);

        CHECK(v1.size() == 2);
        CHECK(v1.capacity() == 5);
        CHECK(v1[0] == 1);
        CHECK(v1[1] == 1);
    }

    SECTION("copy_construction_calls") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v1(2);
        v1.reserve(5);

        construction_test_type::reset();
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v2(v1);        
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 2);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("copy_construction_values") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v1(2, 1);
        v1.reserve(5);

        dtm::vec<int,dtm::detail::move_buffer<int>> v2(v1);
        CHECK(v2.size() == 2);
        CHECK(v2.capacity() == 2);
        CHECK(v2[0] == 1);
        CHECK(v2[1] == 1);

        CHECK(v1.size() == 2);
        CHECK(v1.capacity() == 5);
        CHECK(v1[0] == 1);
        CHECK(v1[1] == 1);
    }

    SECTION("copy_construction_calls") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v1(2);
        v1.reserve(5);

        construction_test_type::reset();
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v2(v1);        
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 2);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("move_construction_values") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v1(2, 1);
        v1.reserve(5);

        dtm::vec<int,dtm::detail::move_buffer<int>> v2(std::move(v1));
        CHECK(v2.size() == 2);
        CHECK(v2[0] == 1);
        CHECK(v2[1] == 1);

        CHECK(v1.size() == 0);
        CHECK(v1.capacity() == 0);
        CHECK(v1.empty());
    }

    SECTION("move_construction_calls") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v1(2);
        v1.reserve(5);

        construction_test_type::reset();
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v2(std::move(v1));       
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("from_initializer_list") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v{0, 1, 2, 3, 4};
        CHECK(v.size() == 5);
        CHECK(v[0] == 0);
        CHECK(v[1] == 1);
        CHECK(v[2] == 2);
        CHECK(v[3] == 3);
        CHECK(v[4] == 4);

        CHECK(v.front() == 0);
        CHECK(v.back() == 4);
    }

    SECTION("from_input_iterator") {
        std::vector<int> base_vec{0,1,2,3,4};
        dtm::vec<int,dtm::detail::move_buffer<int>> v(std::begin(base_vec), std::end(base_vec));
        CHECK(v.size() == 5);
        CHECK(v[0] == 0);
        CHECK(v[1] == 1);
        CHECK(v[2] == 2);
        CHECK(v[3] == 3);
        CHECK(v[4] == 4);
    }
}

TEST_CASE("vec_move_buffer_reserve", "[vec]")
{
    SECTION("from_empty") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec;

        construction_test_type::reset();
        vec.reserve(5);

        CHECK(vec.size() == 0);
        CHECK(vec.empty());
        CHECK(vec.capacity() == 5);

        // There should be a single move construction and a single destruction corresponding
        // to the move of the value during the reallocation and the destruction of the value
        // in the original list.
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);        
    }

    SECTION("from_non_empty") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(1);

        construction_test_type::reset();
        vec.reserve(5);

        CHECK(vec.size() == 1);
        CHECK(!vec.empty());
        CHECK(vec.capacity() == 5);

        // There should be a single move construction and a single destruction corresponding
        // to the move of the value during the reallocation and the destruction of the value
        // in the original list.
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 1);
        CHECK(construction_test_type::num_destructions == 1);
    }
}

TEST_CASE("vec_move_buffer_resize", "[vec]")
{
    SECTION("shrink") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(10);
        REQUIRE(vec.size() == 10);
        REQUIRE(vec.capacity() == 10);

        construction_test_type::reset();
        vec.resize(5);

        CHECK(vec.size() == 5);
        CHECK(vec.capacity() == 10);
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 5);
    }

    SECTION("shrink_to_empty") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(5);
        REQUIRE(vec.size() == 5);
        REQUIRE(vec.capacity() == 5);

        construction_test_type::reset();
        vec.resize(0);

        CHECK(vec.size() == 0);
        CHECK(vec.empty());
        CHECK(vec.capacity() == 5);
        CHECK(construction_test_type::num_destructions == 5);
    }

    SECTION("grow_within_capacity") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(5);
        vec.reserve(10);

        construction_test_type::reset();
        vec.resize(10);

        CHECK(vec.size() == 10);
        CHECK(vec.capacity() == 10);
        CHECK(construction_test_type::num_default_constructions == 5);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("grow_beyond_capacity") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(5);

        construction_test_type::reset();
        vec.resize(10);

        CHECK(vec.size() == 10);
        CHECK(vec.capacity() == 10);
        CHECK(construction_test_type::num_default_constructions == 5);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 5);
        CHECK(construction_test_type::num_destructions == 5);
    }

    SECTION("grow_with_non_default_construction") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(5);

        construction_test_type::reset();
        vec.resize(10, 1, 2);
        CHECK(vec.size() == 10);
        CHECK(vec.capacity() == 10);
        CHECK(construction_test_type::num_non_default_constructions == 5);
    }
}

TEST_CASE("vec_move_buffer_shrink_to_fit", "[vec]") {
    dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(10);
    vec.resize(5);
    REQUIRE(vec.capacity() == 10);

    construction_test_type::reset();
    vec.shrink_to_fit();
    CHECK(vec.capacity() == 5);
    CHECK(construction_test_type::num_default_constructions == 0);
    CHECK(construction_test_type::num_copy_constructions == 0);
    CHECK(construction_test_type::num_move_constructions == 5);
    CHECK(construction_test_type::num_destructions == 5);
}

TEST_CASE("vec_move_buffer_clear", "[vec]") {
    SECTION("from_empty") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v;
        v.clear();
        CHECK(v.empty());
        CHECK(v.size() == 0);
        CHECK(v.capacity() == 0);
    }

    SECTION("from_non_empty") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> vec(5);

        construction_test_type::reset();
        vec.clear();
        CHECK(vec.empty());
        CHECK(vec.size() == 0);
        CHECK(vec.capacity() == 5);
    }
}

TEST_CASE("vec_move_buffer_push_back", "[vec]") {
    SECTION("many_push_backs") {
        int N = 10000;
        dtm::vec<int,dtm::detail::move_buffer<int>> v;

        for (int i = 0; i < N; i++)
            v.push_back(i);
        
        CHECK(!v.empty());
        CHECK(v.capacity() < 2*N + dtm::vec<int,dtm::detail::move_buffer<int>>::minimum_growth_size);
        REQUIRE(v.size() == N);
        for (int i = 0; i < N; i++)
            CHECK(v[i] == i);
    }

    SECTION("copy") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v;
        v.reserve(5);

        construction_test_type test_object;
        construction_test_type::reset();
        for (int i = 0; i < 5; i++)
            v.push_back(test_object);
        
        CHECK(!v.empty());
        CHECK(v.size() == 5);
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 5);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("move") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v;
        v.reserve(5);

        construction_test_type::reset();
        for (int i = 0; i < 5; i++)
            v.push_back(construction_test_type());
        
        CHECK(!v.empty());
        CHECK(v.size() == 5);
        CHECK(construction_test_type::num_default_constructions == 5);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 5);
        CHECK(construction_test_type::num_destructions == 5);
    }

    SECTION("emplace") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v;
        v.reserve(5);

        construction_test_type::reset();
        for (int i = 0; i < 5; i++)
            v.emplace_back(1, 2);
        
        CHECK(!v.empty());
        CHECK(v.size() == 5);
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_non_default_constructions == 5);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);
    }
}

TEST_CASE("vec_move_buffer_pop_back", "[vec]")
{
    SECTION("many_pops") {
        int N = 10000;
        dtm::vec<int,dtm::detail::move_buffer<int>> v(N);

        size_t initial_capacity = v.capacity();
        
        for (int i = 0; i < N; i++) {
            REQUIRE(!v.empty());
            v.pop_back();
        }
        
        CHECK(v.empty());
        CHECK(v.size() == 0);
        CHECK(v.capacity() == initial_capacity);
    }

    SECTION("calls_destructor") {
        dtm::vec<construction_test_type,dtm::detail::move_buffer<construction_test_type>> v(1);

        construction_test_type::reset();
        v.pop_back();

        CHECK(v.empty());
        CHECK(v.size() == 0);
        CHECK(construction_test_type::num_destructions == 1);
    }   
}

TEST_CASE("vec_move_buffer_forward_iterator", "[vec]")
{
    SECTION("empty") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v;
        REQUIRE(v.begin() == v.end());
    }

    SECTION("non_empty") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v({1,2,3});
        std::vector<int> std_v(v.begin(), v.end());
        REQUIRE(std_v.size() == 3);
        CHECK(std_v[0] == 1);
        CHECK(std_v[1] == 2);
        CHECK(std_v[2] == 3);
    }

    SECTION("non_empty_const") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v({1,2,3});
        const auto& c_v = v;
        std::vector<int> std_v(c_v.begin(), c_v.end());
        REQUIRE(std_v.size() == 3);
        CHECK(std_v[0] == 1);
        CHECK(std_v[1] == 2);
        CHECK(std_v[2] == 3);
    }

    SECTION("output") {
        dtm::vec<int,dtm::detail::move_buffer<int>> vec(5);
        int i = 0;
        for (auto& val : vec)
            val = i++;

        REQUIRE(vec.size() == 5);
        CHECK(vec[0] == 0);
        CHECK(vec[1] == 1);
        CHECK(vec[2] == 2);
        CHECK(vec[3] == 3);
        CHECK(vec[4] == 4);
    }
}

TEST_CASE("vec_move_buffer_insert", "[vec]")
{
    SECTION("copy_into_empty") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v;
        v.insert(v.end(), 0);
        REQUIRE(v.size() == 1);
        CHECK(v[0] == 0);
    }

    SECTION("move_into_empty") {
        dtm::vec<std::unique_ptr<int>> v;
        v.insert(v.end(), std::make_unique<int>(0));
        REQUIRE(v.size() == 1);
        CHECK(*v[0] == 0);
    }

    SECTION("copy_into_beginning") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v{1,2,3,4};
        v.insert(v.begin(), 0);
        REQUIRE(v.size() == 5);
        for (int i = 0; i < 5; i++)
            CHECK(v[i] == i);        
    }

    SECTION("copy_into_end") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v{0,1,2,3};
        v.insert(v.end(), 4);
        REQUIRE(v.size() == 5);
        for (int i = 0; i < 5; i++)
            CHECK(v[i] == i);
    }        

    SECTION("copy_into_middle") {
        dtm::vec<int,dtm::detail::move_buffer<int>> v{0,1,3,4};
        v.insert(v.begin() + 2, 2);
        REQUIRE(v.size() == 5);
        for (int i = 0; i < 5; i++)
            CHECK(v[i] == i);
    }        
}


