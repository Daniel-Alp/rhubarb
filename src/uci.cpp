#include "board.h"
#include "move.h"
#include "makemove.h"
#include "movegen.h"
#include "perft.h"
#include "parser.h"
#include "search.h"
#include "timemanagement.h"
#include "transposition.h"
#include "types.h"
#include "uci.h"
#include <iostream>
#include <string>
#include <thread>
#include <vector>

void uci_loop() {
	Position pos{};
	SearchData search_data{};
	search_data.nodes = 0;
	std::thread search_thread;

	std::string cmd;
	std::string cmd_type;
	std::vector<std::string> cmd_sections;

	while (std::getline(std::cin, cmd)) {
		cmd_sections = split_string(cmd, ' ');
		cmd_type = cmd_sections[0];

		if (cmd_type == "quit") {
			if (search_thread.joinable()) {
				search_thread.join();
			}
			break;
		}
		else if (cmd_type == "uci") {
			std::cout << "id name rhubarb" << std::endl;
			std::cout << "id author Daniel Alp" << std::endl;
			std::cout << "uciok" << std::endl;
		}
		else if (cmd_type == "isready") {
			std::cout << "readyok" << std::endl;
		}
		else if (cmd_type == "ucinewgame") {
			pos = load_from_fen(start_fen);
			hash_table.clear();
			clear_history_table();
		}
		else if (cmd_type == "position") {
			uci_position_command(cmd_sections, pos);
		} 
		else if (cmd_type == "perft") {
			uci_perft_command(cmd_sections, pos);
		}
		else if (cmd_type == "go") {
			if (search_thread.joinable()) {
				search_thread.join();
			}
			uci_go_command(cmd_sections, search_thread, search_data, pos);
		}
		else if (cmd_type == "stop") {
			search_data.searching = false;
			if (search_thread.joinable()) {
				search_thread.join();
			}
		}
		else {
			std::cout << "Unknown command: " << cmd << std::endl;
		}
	}
}

void uci_go_command(const std::vector<std::string>& cmd_sections, std::thread& search_thread, SearchData& search_data, Position& pos) {
	i32 wtime = 0;
	i32 btime = 0;
	i32 winc = 0;
	i32 binc = 0;
	i32 moves_to_go = 0;

	std::string token_type;
	
	for (i32 token = 1; token < cmd_sections.size() - 1; token += 2) {
		token_type = cmd_sections[token];
		if (token_type == "wtime") {
			wtime = std::stoi(cmd_sections[token + 1]);
		}
		else if (token_type == "btime") {
			btime = std::stoi(cmd_sections[token + 1]);
		}
		else if (token_type == "winc") {
			winc = std::stoi(cmd_sections[token + 1]);
		}
		else if (token_type == "binc") {
			binc = std::stoi(cmd_sections[token + 1]);
		}
		else if (token_type == "movestogo") {
			moves_to_go = std::stoi(cmd_sections[token + 1]);
		}
	}

	i32 player_time = 0;
	i32 opp_time = 0;
	i32 player_inc = 0;
	i32 opp_inc = 0;

	if (pos.side_to_move == Color::WHITE) {
		player_time = wtime;
		opp_time = btime;
		player_inc = winc;
		opp_inc = binc;
	}
	else {
		player_time = btime;
		opp_time = wtime;
		player_inc = binc;
		opp_inc = winc;
	}
	search_data.start_time = get_current_time();
	search_data.time_allotted = get_time_allotted(player_time, opp_time, player_inc, opp_inc, moves_to_go);
	search_data.max_depth = 255;

	search_thread = std::thread(best_move, std::ref(pos), std::ref(search_data));
}

void uci_perft_command(const std::vector<std::string>& cmd_sections, Position& pos) {
	i32 depth = std::stoi(cmd_sections[1]);
	i32 ply = 0;
	std::cout << perft(pos, depth, ply) << std::endl;
}

void uci_position_command(const std::vector<std::string>& cmd_sections, Position& pos) {
	i32 move_token = 0;

	if (cmd_sections[1] == "startpos") {
		pos = load_from_fen(start_fen);
		move_token = 3;
	}
	else {
		std::string fen = "";
		for (i32 token = 2; token < 8; token++) {
			fen += cmd_sections[token];
			fen += " ";
		}
		pos = load_from_fen(fen);
		move_token = 9;
	}

	while (move_token < cmd_sections.size()) {
		MoveList move_list = gen_pseudo_moves(pos, false);
		for (i32 i = 0; i < move_list.size(); i++) {
			Move move = move_list.get(i);
			if (move.to_str() == cmd_sections[move_token]) {
				make_move(pos, move);
			}
		}
		move_token++;
	}
}