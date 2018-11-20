// move_buffer_test.cpp
//

#include "dtm/detail/move_buffer.hpp"

#include "construction_test_type.hpp"
#include "move_only_tracking_type.hpp"

#include "catch.hpp"

TEST_CASE("move_buffer_constructor", "[buffer]")
{
    SECTION("constructor_and_destructor_calls") {
        construction_test_type::reset();

        dtm::detail::move_buffer<construction_test_type> buffer;
        REQUIRE(construction_test_type::num_default_constructions == 0);
        REQUIRE(construction_test_type::num_copy_constructions == 0);
        REQUIRE(construction_test_type::num_move_constructions == 0);
        REQUIRE(construction_test_type::num_destructions == 0);

        buffer.reallocate(1);
        REQUIRE(construction_test_type::num_default_constructions == 0);
        REQUIRE(construction_test_type::num_copy_constructions == 0);
        REQUIRE(construction_test_type::num_move_constructions == 0);
        REQUIRE(construction_test_type::num_destructions == 0);

        buffer.construct(buffer.end++);
        REQUIRE(construction_test_type::num_default_constructions == 1);
        REQUIRE(construction_test_type::num_copy_constructions == 0);
        REQUIRE(construction_test_type::num_move_constructions == 0);
        REQUIRE(construction_test_type::num_destructions == 0);

        buffer.reallocate(2);
        REQUIRE(construction_test_type::num_default_constructions == 1);
        REQUIRE(construction_test_type::num_copy_constructions == 0);
        REQUIRE(construction_test_type::num_move_constructions == 1);
        REQUIRE(construction_test_type::num_destructions == 1);

        construction_test_type dummy;
        REQUIRE(construction_test_type::num_default_constructions == 2);
        buffer.construct(buffer.end++, dummy);
        REQUIRE(construction_test_type::num_default_constructions == 2);
        REQUIRE(construction_test_type::num_copy_constructions == 1);
        REQUIRE(construction_test_type::num_move_constructions == 1);
        REQUIRE(construction_test_type::num_destructions == 1);
    }

    SECTION("inner_destructors_called_on_buffer_destruct") {
        construction_test_type::reset();

        {
            dtm::detail::move_buffer<construction_test_type> buffer;
            buffer.reallocate(1);
            buffer.construct(buffer.end++);
        }

        REQUIRE(construction_test_type::num_destructions == 1);
    }

    SECTION("constructor_and_destructor_calls") {
        construction_test_type::reset();
        dtm::detail::move_buffer<construction_test_type> buffer;
        buffer.reallocate(1);
        buffer.construct(buffer.end++);
        REQUIRE(construction_test_type::num_default_constructions == 1);        
        buffer.destruct(--buffer.end);
        REQUIRE(construction_test_type::num_destructions == 1);

        construction_test_type obj_to_move;
        buffer.construct(buffer.end++, std::move(obj_to_move));
        REQUIRE(construction_test_type::num_move_constructions == 1);
    }

    SECTION("empty_copy_and_move_construction") {
        construction_test_type::reset();
        dtm::detail::move_buffer<construction_test_type> buffer;
        dtm::detail::move_buffer<construction_test_type> copy_of_buffer(buffer);

        REQUIRE(construction_test_type::num_default_constructions == 0);
        REQUIRE(construction_test_type::num_copy_constructions == 0);
        REQUIRE(construction_test_type::num_move_constructions == 0);
        REQUIRE(construction_test_type::num_destructions == 0);    

        REQUIRE(copy_of_buffer.begin == nullptr);
        REQUIRE(copy_of_buffer.end == nullptr);
        REQUIRE(copy_of_buffer.capacity == 0);

        dtm::detail::move_buffer<construction_test_type> move_of_buffer(std::move(buffer));

        REQUIRE(construction_test_type::num_default_constructions == 0);
        REQUIRE(construction_test_type::num_copy_constructions == 0);
        REQUIRE(construction_test_type::num_move_constructions == 0);
        REQUIRE(construction_test_type::num_destructions == 0);    

        REQUIRE(move_of_buffer.begin == nullptr);
        REQUIRE(move_of_buffer.end == nullptr);
        REQUIRE(move_of_buffer.capacity == 0);
    }

    SECTION("nonempty_copy_construction") {
        construction_test_type::reset();
        dtm::detail::move_buffer<construction_test_type> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++);

        dtm::detail::move_buffer<construction_test_type> copy_of_buffer(buffer);
        REQUIRE(construction_test_type::num_default_constructions == 1);
        REQUIRE(construction_test_type::num_copy_constructions == 1);
        REQUIRE(construction_test_type::num_destructions == 0);

        REQUIRE(buffer.end - buffer.begin == 1);
        REQUIRE(buffer.capacity == 2);
        REQUIRE(copy_of_buffer.end - copy_of_buffer.begin == 1);
        REQUIRE(copy_of_buffer.capacity == 1);
    }

    SECTION("nonempty_move_construction") {
        construction_test_type::reset();
        dtm::detail::move_buffer<construction_test_type> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++);

        dtm::detail::move_buffer<construction_test_type> move_of_buffer(std::move(buffer));
        REQUIRE(construction_test_type::num_default_constructions == 1);
        REQUIRE(construction_test_type::num_move_constructions == 0);
        REQUIRE(construction_test_type::num_destructions == 0);

        REQUIRE(buffer.end == nullptr);
        REQUIRE(buffer.begin == nullptr);
        REQUIRE(buffer.capacity == 0);
        REQUIRE(move_of_buffer.end - move_of_buffer.begin == 1);
        REQUIRE(move_of_buffer.capacity == 2);
    }    

    SECTION("move_only") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(1);
        buffer.construct(buffer.end++, 0);
        move_only_tracking_type* original_ptr = buffer.begin;

        buffer.reallocate(2);
        REQUIRE_FALSE(buffer.begin->move_assigned);
        REQUIRE(buffer.begin->moved_from == original_ptr);
    }
}

TEST_CASE("move_buffer_reallocate", "[buffer]") {
    SECTION("grow") {
        dtm::detail::move_buffer<int> buffer;

        REQUIRE(buffer.begin == nullptr);
        REQUIRE(buffer.end == nullptr);
        REQUIRE(buffer.capacity == 0);

        buffer.reallocate(1);
        REQUIRE(buffer.begin != nullptr);
        REQUIRE((buffer.end - buffer.begin) == 0);
        REQUIRE(buffer.capacity == 1);

        buffer.construct(buffer.end++, 1);
        buffer.reallocate(2);

        REQUIRE(buffer.begin != nullptr);
        REQUIRE((buffer.end - buffer.begin) == 1);
        REQUIRE(buffer.capacity == 2);
        REQUIRE(*buffer.begin == 1);
    }

    SECTION("shrink") {
        dtm::detail::move_buffer<int> buffer;
        buffer.reallocate(10);

        REQUIRE(buffer.begin != nullptr);
        REQUIRE((buffer.end - buffer.begin) == 0);
        REQUIRE(buffer.capacity == 10);

        for (int i = 0; i < 10; i++) {
            buffer.construct(buffer.end++, i);
        }

        buffer.reallocate(5);
        
        REQUIRE(buffer.begin != nullptr);
        REQUIRE((buffer.end - buffer.begin) == 5);
        REQUIRE(buffer.capacity == 5);

        for (int i = 0; i < 5; i++)
            REQUIRE(buffer.begin[i] == i);
    }
}

TEST_CASE("move_buffer_shift_right", "[buffer]") {
    SECTION("1_into_empty_no_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        bool is_constructed = buffer.shift_right(0, 1); 

        CHECK_FALSE(is_constructed);
        CHECK(buffer.end - buffer.begin == 1);
        CHECK(buffer.capacity == 1);       
    }

    SECTION("1_into_begin_no_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(1);
        buffer.construct(buffer.end++, 0);

        bool is_constructed = buffer.shift_right(0, 1);
        
        CHECK_FALSE(is_constructed);
        CHECK(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK(buffer.begin[1].value == 0);
    }

    SECTION("1_into_end_no_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(1);
        buffer.construct(buffer.end++, 0);

        bool is_constructed = buffer.shift_right(1, 1);
        
        CHECK_FALSE(is_constructed);
        CHECK(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK(buffer.begin[0].value == 0);
    }

    SECTION("many_into_middle_no_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(5);
        for (int i = 0; i < 5; i++)
            buffer.construct(buffer.end++, i);
        
        move_only_tracking_type* old_begin = buffer.begin; // Have to keep a copy of this, since it'll get reallocated.
        bool is_constructed = buffer.shift_right(2, 3);

        // Elements should already be constructed
        CHECK_FALSE(is_constructed); 

        // Should be 8 elements in buffer
        CHECK(buffer.end - buffer.begin == 8);
        CHECK(buffer.capacity == 8);

        for (int i = 2; i < 5; i++) {
            CHECK(buffer.begin[i + 3].value == i);
            CHECK(buffer.begin[i + 3].move_assigned == ((i + 3) < 5));
            CHECK(buffer.begin[i + 3].moved_from == &old_begin[i]);
        }
    }

    SECTION("1_into_empty_existing_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(2);

        bool is_constructed = buffer.shift_right(0, 1);
        
        CHECK_FALSE(is_constructed);
        CHECK(buffer.end - buffer.begin == 1);
        CHECK(buffer.capacity == 2);
    }

    SECTION("1_into_end_existing_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++, 1);

        bool is_constructed = buffer.shift_right(1, 1);
        
        CHECK_FALSE(is_constructed);
        CHECK(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK(buffer.begin[0].moved_from == nullptr);
    }

    SECTION("many_into_end_existing_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(10);
        buffer.construct(buffer.end++, 1);

        bool is_constructed = buffer.shift_right(1, 5);

        CHECK_FALSE(is_constructed);
        CHECK(buffer.end - buffer.begin == 6);
        CHECK(buffer.capacity == 10);
        CHECK(buffer.begin[0].moved_from == nullptr);
    }

    SECTION("1_into_begin_existing_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++, 9);

        bool is_constructed = buffer.shift_right(0, 1);

        CHECK(is_constructed);
        CHECK(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK_FALSE(buffer.begin[1].move_assigned);
        CHECK(buffer.begin[1].moved_from == buffer.begin);
        CHECK(buffer.begin[1].value == 9);
    }

    SECTION("1_into_begin_chained_existing_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(3);
        buffer.construct(buffer.end++, 0);
        buffer.construct(buffer.end++, 1);

        bool is_constructed = buffer.shift_right(0, 1);

        CHECK(is_constructed);

        // Buffer should now have 3 constructed elements.
        CHECK(buffer.end - buffer.begin == 3);
        CHECK(buffer.capacity == 3);

        // The first value should have been move assigned to, since it was
        // previously constructed.
        CHECK(buffer.begin[1].move_assigned);
        CHECK(buffer.begin[1].moved_from == buffer.begin);
        CHECK(buffer.begin[1].value == 0);

        // The second value should have been move constructed, since it was
        // not previously constructed.
        CHECK_FALSE(buffer.begin[2].move_assigned);
        CHECK(buffer.begin[2].moved_from == buffer.begin + 1);
        CHECK(buffer.begin[2].value == 1);
    }

    SECTION("many_into_begin_chained_existing_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(10);
        for (int i = 0; i < 5; i++)
            buffer.construct(buffer.end++, i);
        
        bool is_constructed = buffer.shift_right(0, 3);

        // Elements should already be constructed
        CHECK(is_constructed); 

        // Should be 8 elements in buffer
        CHECK(buffer.end - buffer.begin == 8);
        CHECK(buffer.capacity == 10);

        for (int i = 0; i < 5; i++) {
            CHECK(buffer.begin[i + 3].value == i);
            CHECK(buffer.begin[i + 3].move_assigned == ((i + 3) < 5));
            CHECK(buffer.begin[i + 3].moved_from == &buffer.begin[i]);
        }
    }

    SECTION("many_into_middle_chained_existing_capacity") {
        dtm::detail::move_buffer<move_only_tracking_type> buffer;
        buffer.reallocate(10);
        for (int i = 0; i < 5; i++)
            buffer.construct(buffer.end++, i);
        
        bool is_constructed = buffer.shift_right(2, 3);

        // Elements should already be constructed
        CHECK(is_constructed); 

        // Should be 8 elements in buffer
        CHECK(buffer.end - buffer.begin == 8);
        CHECK(buffer.capacity == 10);

        for (int i = 2; i < 5; i++) {
            CHECK(buffer.begin[i + 3].value == i);
            CHECK(buffer.begin[i + 3].move_assigned == ((i + 3) < 5));
            CHECK(buffer.begin[i + 3].moved_from == &buffer.begin[i]);
        }
    }
}