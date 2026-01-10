#include <tracker_allocator.hpp>

typedef tracker_allocator_counter counter_type;

counter_type::size_type counter_type::allocationCount_ = 0;

counter_type::size_type counter_type::deallocationCount_ = 0;

int counter_type::constructCount_ = 0;
int counter_type::destructCount_ = 0;