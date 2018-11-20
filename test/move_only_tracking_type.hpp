// move_only_tracking_type.hpp
//
// A simple move-only type used in datum container tests to track moves
//

#ifndef INCLUDED_DATUM_TEST_MOVE_ONLY_TRACKING_TYPE
#define INCLUDED_DATUM_TEST_MOVE_ONLY_TRACKING_TYPE

struct move_only_tracking_type {
    move_only_tracking_type(int val) { value = val; }

    // move only
    move_only_tracking_type(const move_only_tracking_type&) = delete;
    move_only_tracking_type& operator=(const move_only_tracking_type&) = delete;

    move_only_tracking_type(move_only_tracking_type&& rhs) noexcept {
        value = rhs.value;
        move_assigned = false;
        moved_from = &rhs;
    }

    move_only_tracking_type& operator=(move_only_tracking_type&& rhs) noexcept {
        value = rhs.value;
        move_assigned = true;
        moved_from = &rhs;            
    }

    int value;
    bool move_assigned = false;
    move_only_tracking_type* moved_from = nullptr;
};

#endif //INCLUDED_DATUM_TEST_MOVE_ONLY_TRACKING_TYPE
