// relocate_buffer_test.cpp
//

#include "dtm/detail/relocate_buffer.hpp"

#include "construction_test_type.hpp"
#include "move_only_tracking_type.hpp"

#include "catch.hpp"

TEST_CASE("relocate_buffer_constructor", "[buffer]")
{
    SECTION("buffer_has_right_size") {
        dtm::detail::relocate_buffer<int> buffer;
        CHECK(reinterpret_cast<char*>(buffer.end) - reinterpret_cast<char*>(buffer.begin) == 0);

        buffer.reallocate(1);
        CHECK(reinterpret_cast<char*>(buffer.end) - reinterpret_cast<char*>(buffer.begin) == 0);
    }

    SECTION("constructor_and_destructor_calls") {
        construction_test_type::reset();
        dtm::detail::relocate_buffer<construction_test_type> buffer;
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);

        construction_test_type::reset();
        buffer.reallocate(1);
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);

        construction_test_type::reset();
        buffer.construct(buffer.end++);
        CHECK(construction_test_type::num_default_constructions == 1);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);

        construction_test_type::reset();
        buffer.reallocate(2);
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);

        construction_test_type dummy;
        construction_test_type::reset();
        buffer.construct(buffer.end++, dummy);
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 1);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);

        construction_test_type::reset();
        buffer.construct(buffer.end++, std::move(dummy));
        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 1);
        CHECK(construction_test_type::num_destructions == 0);
    }

    SECTION("inner_destructors_called_on_buffer_destruct") {
        construction_test_type::reset();
        {
            dtm::detail::relocate_buffer<construction_test_type> buffer;
        }
        CHECK(construction_test_type::num_destructions == 0);

        construction_test_type::reset();
        {
            dtm::detail::relocate_buffer<construction_test_type> buffer;
            buffer.reallocate(1);
            CHECK(construction_test_type::num_destructions == 0);
        }
        CHECK(construction_test_type::num_destructions == 0);

        construction_test_type::reset();
        {
            dtm::detail::relocate_buffer<construction_test_type> buffer;
            buffer.reallocate(1);
            buffer.construct(buffer.end++);
        }
        CHECK(construction_test_type::num_destructions == 1);
    }

    SECTION("constructor_and_destructor_calls") {
        construction_test_type::reset();
        dtm::detail::relocate_buffer<construction_test_type> buffer;
        buffer.reallocate(1);
        buffer.construct(buffer.end++);
        CHECK(construction_test_type::num_default_constructions == 1);        
        buffer.destruct(--buffer.end);
        CHECK(construction_test_type::num_destructions == 1);

        construction_test_type obj_to_move;
        buffer.construct(buffer.end++, std::move(obj_to_move));
        CHECK(construction_test_type::num_move_constructions == 1);
    }

    SECTION("empty_copy_and_move_construction") {
        construction_test_type::reset();
        dtm::detail::relocate_buffer<construction_test_type> buffer;
        dtm::detail::relocate_buffer<construction_test_type> copy_of_buffer(buffer);

        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);    

        CHECK(copy_of_buffer.begin == nullptr);
        CHECK(copy_of_buffer.end == nullptr);
        CHECK(copy_of_buffer.capacity == 0);

        dtm::detail::relocate_buffer<construction_test_type> move_of_buffer(std::move(buffer));

        CHECK(construction_test_type::num_default_constructions == 0);
        CHECK(construction_test_type::num_copy_constructions == 0);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);    

        CHECK(move_of_buffer.begin == nullptr);
        CHECK(move_of_buffer.end == nullptr);
        CHECK(move_of_buffer.capacity == 0);
    }

    SECTION("nonempty_copy_construction") {
        construction_test_type::reset();
        dtm::detail::relocate_buffer<construction_test_type> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++);

        dtm::detail::relocate_buffer<construction_test_type> copy_of_buffer(buffer);
        CHECK(construction_test_type::num_default_constructions == 1);
        CHECK(construction_test_type::num_copy_constructions == 1);
        CHECK(construction_test_type::num_destructions == 0);

        CHECK(buffer.end - buffer.begin == 1);
        CHECK(buffer.capacity == 2);
        CHECK(copy_of_buffer.end - copy_of_buffer.begin == 1);
        CHECK(copy_of_buffer.capacity == 1);
    }

    SECTION("nonempty_move_construction") {
        construction_test_type::reset();
        dtm::detail::relocate_buffer<construction_test_type> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++);

        dtm::detail::relocate_buffer<construction_test_type> move_of_buffer(std::move(buffer));
        CHECK(construction_test_type::num_default_constructions == 1);
        CHECK(construction_test_type::num_move_constructions == 0);
        CHECK(construction_test_type::num_destructions == 0);

        CHECK(buffer.end == nullptr);
        CHECK(buffer.begin == nullptr);
        CHECK(buffer.capacity == 0);
        CHECK(move_of_buffer.end - move_of_buffer.begin == 1);
        CHECK(move_of_buffer.capacity == 2);
    }    
}

TEST_CASE("relocate_buffer_reallocate", "[buffer]") {
    SECTION("grow") {
        dtm::detail::relocate_buffer<int> buffer;

        CHECK(buffer.begin == nullptr);
        CHECK(buffer.end == nullptr);
        CHECK(buffer.capacity == 0);

        buffer.reallocate(1);
        CHECK(buffer.begin != nullptr);
        CHECK((buffer.end - buffer.begin) == 0);
        CHECK(buffer.capacity == 1);

        buffer.construct(buffer.end++, 1);
        buffer.reallocate(2);

        CHECK(buffer.begin != nullptr);
        REQUIRE((buffer.end - buffer.begin) == 1);
        CHECK(buffer.capacity == 2);
        CHECK(*buffer.begin == 1);
    }

    SECTION("shrink") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(10);

        CHECK(buffer.begin != nullptr);
        CHECK((buffer.end - buffer.begin) == 0);
        CHECK(buffer.capacity == 10);

        for (int i = 0; i < 10; i++) {
            buffer.construct(buffer.end++, i);
        }

        buffer.reallocate(5);
        
        CHECK(buffer.begin != nullptr);
        REQUIRE((buffer.end - buffer.begin) == 5);
        CHECK(buffer.capacity == 5);

        for (int i = 0; i < 5; i++)
            CHECK(buffer.begin[i] == i);
    }
}

TEST_CASE("relocate_buffer_shift_right", "[buffer]") {
    SECTION("1_into_empty_no_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        bool is_constructed = buffer.shift_right(0, 1); 

        CHECK_FALSE(is_constructed);
        CHECK(buffer.end - buffer.begin == 1);
        CHECK(buffer.capacity == 1);       
    }

    SECTION("1_into_begin_no_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(1);
        buffer.construct(buffer.end++, 0);

        bool is_constructed = buffer.shift_right(0, 1);
        
        CHECK_FALSE(is_constructed);
        REQUIRE(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK(buffer.begin[1] == 0);
    }

    SECTION("1_into_end_no_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(1);
        buffer.construct(buffer.end++, 0);

        bool is_constructed = buffer.shift_right(1, 1);
        
        CHECK_FALSE(is_constructed);
        REQUIRE(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK(buffer.begin[0] == 0);
    }

    SECTION("many_into_middle_no_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(5);
        for (int i = 0; i < 5; i++)
            buffer.construct(buffer.end++, i);
        
        int* old_begin = buffer.begin; // Have to keep a copy of this, since it'll get reallocated.
        bool is_constructed = buffer.shift_right(2, 3);

        // Elements should already be constructed
        CHECK_FALSE(is_constructed); 

        // Should be 8 elements in buffer
        REQUIRE(buffer.end - buffer.begin == 8);
        CHECK(buffer.capacity == 8);

        for (int i = 2; i < 5; i++)
            CHECK(buffer.begin[i + 3] == i);
    }

    SECTION("1_into_empty_existing_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(2);

        bool is_constructed = buffer.shift_right(0, 1);
        
        CHECK_FALSE(is_constructed);
        REQUIRE(buffer.end - buffer.begin == 1);
        CHECK(buffer.capacity == 2);
    }

    SECTION("1_into_end_existing_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++, 1);

        bool is_constructed = buffer.shift_right(1, 1);
        
        CHECK_FALSE(is_constructed);
        REQUIRE(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK(buffer.begin[0] == 1);
    }

    SECTION("many_into_end_existing_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(10);
        buffer.construct(buffer.end++, 1);

        bool is_constructed = buffer.shift_right(1, 5);

        CHECK_FALSE(is_constructed);
        REQUIRE(buffer.end - buffer.begin == 6);
        CHECK(buffer.capacity == 10);
        CHECK(buffer.begin[0] == 1);
    }

    SECTION("1_into_begin_existing_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(2);
        buffer.construct(buffer.end++, 9);

        bool is_constructed = buffer.shift_right(0, 1);

        CHECK_FALSE(is_constructed);
        REQUIRE(buffer.end - buffer.begin == 2);
        CHECK(buffer.capacity == 2);
        CHECK(buffer.begin[1] == 9);
    }

    SECTION("1_into_begin_chained_existing_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(3);
        buffer.construct(buffer.end++, 0);
        buffer.construct(buffer.end++, 1);

        bool is_constructed = buffer.shift_right(0, 1);

        CHECK_FALSE(is_constructed);

        // Buffer should now have 3 constructed elements.
        REQUIRE(buffer.end - buffer.begin == 3);
        CHECK(buffer.capacity == 3);
        CHECK(buffer.begin[1] == 0);
        CHECK(buffer.begin[2] == 1);
    }

    SECTION("many_into_begin_chained_existing_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(10);
        for (int i = 0; i < 5; i++)
            buffer.construct(buffer.end++, i);
        
        bool is_constructed = buffer.shift_right(0, 3);

        // Elements should already be constructed
        CHECK_FALSE(is_constructed); 

        // Should be 8 elements in buffer
        REQUIRE(buffer.end - buffer.begin == 8);
        CHECK(buffer.capacity == 10);

        for (int i = 0; i < 5; i++) {
            CHECK(buffer.begin[i + 3] == i);
        }
    }

    SECTION("many_into_middle_chained_existing_capacity") {
        dtm::detail::relocate_buffer<int> buffer;
        buffer.reallocate(10);
        for (int i = 0; i < 5; i++)
            buffer.construct(buffer.end++, i);
        
        bool is_constructed = buffer.shift_right(2, 3);

        // Elements should already be constructed
        CHECK_FALSE(is_constructed); 

        // Should be 8 elements in buffer
        REQUIRE(buffer.end - buffer.begin == 8);
        CHECK(buffer.capacity == 10);

        for (int i = 2; i < 5; i++) {
            CHECK(buffer.begin[i + 3] == i);
        }
    }
}