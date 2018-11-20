// construction_test_type.hpp
//
// A simple type used in datum container tests to check for number of construction/destruction calls
//

#ifndef INCLUDED_DATUM_TEST_CONSTRUCTION_TEST_TYPE_HPP
#define INCLUDED_DATUM_TEST_CONSTRUCTION_TEST_TYPE_HPP

namespace {
    struct construction_test_type {
        static int num_default_constructions;
        static int num_non_default_constructions;
        static int num_destructions;
        static int num_copy_constructions;
        static int num_move_constructions;
        static int num_copy_assignments;
        static int num_move_assignments;

        static void reset() {
            num_default_constructions = 0;
            num_destructions = 0;
            num_copy_constructions = 0;
            num_move_constructions = 0;
            num_copy_assignments = 0;
            num_move_assignments = 0;
            num_non_default_constructions = 0;
        }

        construction_test_type() {
            num_default_constructions += 1;
        }

        construction_test_type(int, int) {
            num_non_default_constructions += 1;
        }

        construction_test_type(const construction_test_type&) {
            num_copy_constructions += 1;
        }

        construction_test_type(construction_test_type&&) noexcept {
            num_move_constructions += 1;
        }

        ~construction_test_type() {
            num_destructions += 1;
        }

        construction_test_type& operator = (const construction_test_type&) {
            num_copy_assignments += 1;
            return *this;
        }

        construction_test_type& operator = (construction_test_type&&) noexcept {
            num_move_assignments += 1;
            return *this;
        }
    };
    
    int construction_test_type::num_default_constructions;
    int construction_test_type::num_non_default_constructions;
    int construction_test_type::num_destructions;
    int construction_test_type::num_copy_constructions;
    int construction_test_type::num_move_constructions;
    int construction_test_type::num_copy_assignments;
    int construction_test_type::num_move_assignments;
}

#endif //INCLUDED_DATUM_TEST_CONSTRUCTION_TEST_TYPE_HPP
