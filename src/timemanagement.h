#pragma once

#include "common.h"
#include <chrono>

i64 get_time_allotted(i64 player_time, i64 opp_time, i64 player_inc, i64 opp_inc, i32 moves_to_go);

inline i64 get_current_time() {
	return std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::steady_clock().now().time_since_epoch()).count();
}