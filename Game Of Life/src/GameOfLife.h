#pragma once

#include <unordered_set>
#include <unordered_map>

class GameOfLife {
public:
	struct Cell {
		int x;
		int y;
	};
private:
	struct cellHash {
		std::size_t operator()(const Cell& v) const {
			std::size_t h1 = std::hash<int>{}(v.x);
			std::size_t h2 = std::hash<int>{}(v.y);
			return h1 ^ (h2 << 1); // Combine the hash values
		}
	};
	struct cellEqual {
		bool operator()(const Cell& v1,const Cell& v2) const {
			return v1.x == v2.x && v1.y == v2.y;
		}
	};
	void markNeighbourCells(const Cell cell) { //oh
		Cell curr_cell = {cell.x-1,cell.y};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
		curr_cell = {cell.x-1,cell.y-1};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
		curr_cell = {cell.x-1,cell.y+1};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
		curr_cell = {cell.x+1,cell.y};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
		curr_cell = {cell.x+1,cell.y-1};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
		curr_cell = {cell.x+1,cell.y+1};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
		curr_cell = {cell.x,cell.y+1};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
		curr_cell = {cell.x,cell.y-1};
		marked_cells.try_emplace(curr_cell,0);
		marked_cells.at(curr_cell)++;
	}
public:
	std::unordered_set<Cell,cellHash,cellEqual> alive_cells;
	std::unordered_set<Cell,cellHash,cellEqual> next_alive_cells;
	std::unordered_map<Cell,int,cellHash,cellEqual> marked_cells;
	void Tick() {
		next_alive_cells.clear();
		marked_cells.clear();
		for(const auto& cell: alive_cells) {
			markNeighbourCells(cell);
		}
		for(const auto& [fst,snd]: marked_cells) {
			if(snd>=2 && snd<=3) {
				if(alive_cells.count(fst)==0 && snd!=3) {
					continue;
				}
				next_alive_cells.insert(fst);
			}
		}
		alive_cells = next_alive_cells;
	}
};