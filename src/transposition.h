#pragma once

#include "common.h"
#include "move.h"
#include "constants.h"
#include <array>

enum class HashFlag {
	EXACT, 
	ALPHA,
	BETA
};

struct HashEntry {
	u64 zobrist_key;
	i32 depth;
	i32 score;
	HashFlag hash_flag;
	Move best_move;
};

constexpr u64 num_hash_entries = 2097152;

class HashTable {
public:
	inline void clear() {
		for (i32 i = 0; i < num_hash_entries; i++) {
			hash_table[i].zobrist_key = 0;
		}
	}

	inline void record(u64 zobrist_key, i32 depth, i32 score, HashFlag hash_flag, Move best_move) {
		const i32 i = zobrist_key % num_hash_entries;
		hash_table[i].zobrist_key = zobrist_key;
		hash_table[i].depth = depth;
		hash_table[i].score = score;
		hash_table[i].hash_flag = hash_flag;
		hash_table[i].best_move = best_move;
	}

	inline HashEntry get(u64 zobrist_key) const {
		return hash_table[zobrist_key % num_hash_entries];
	}

	inline i32 score_to_hash_table(i32 score, i32 ply) const {
		if (score >= mate_score - max_search_ply) {
			return score + ply;
		}
		if (score <= -mate_score + max_search_ply) {
			return score - ply;
		}
		return score;
	} 

	inline i32 hash_table_to_score(i32 score, i32 ply) const {
		if (score >= mate_score - max_search_ply) {
			return score - ply;
		}
		if (score <= -mate_score + max_search_ply) {
			return score + ply;
		}
		return score;
	}

private:
	std::array<HashEntry, num_hash_entries> hash_table;
};

// never lock the TT. data races are better than the cost of locking
inline HashTable hash_table;