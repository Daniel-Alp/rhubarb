#pragma once

#include "board.h"
#include "search.h"
#include <string>
#include <thread>
#include <vector>

void uci_loop();
void uci_go_command(const std::vector<std::string>& cmd_sections, std::thread& search_thread, SearchData& search_data, Position& pos);
void uci_perft_command(const std::vector<std::string>& cmd_sections, Position& pos);
void uci_position_command(const std::vector<std::string>& cmd_sections, Position& pos);