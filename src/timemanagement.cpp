#include "common.h"
#include "timemanagement.h"

i64 get_time_allotted(i64 player_time, i64 opp_time, i64 player_inc, i64 opp_inc, i32 moves_to_go) {
	//Basic time management for now
	i64 time_allotted = (player_time + 1) >> 5;
	return time_allotted;
}

