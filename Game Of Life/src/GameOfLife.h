#pragma once

#include <unordered_set>
#include <unordered_map>
#include <iostream>

class GameOfLife {
public:
	struct Cell {
		Cell(const int x_,const int y_):x(x_),y(y_){}
		Cell():x(0),y(0){}
		int x;
		int y;
		Cell operator+(const Cell& cell) const { return Cell(x+cell.x,y+cell.y); }
		Cell operator-(const Cell& cell) const { return Cell(x-cell.x,y-cell.y); }
		Cell& operator=(const Cell& cell) { x=cell.x;y=cell.y; return *this; }
		Cell& operator+=(const Cell& cell) { x+=cell.x;y+=cell.y; return *this; }
		bool operator==(const Cell& cell) const { return x==cell.x&&y==cell.y; }
		bool operator!=(const Cell& cell) const { return x!=cell.x||y!=cell.y; }
		friend std::ostream& operator<<(std::ostream& os,const Cell& cell) { os<<"[ "<<cell.x<<", "<<cell.y<<"]"; return os; }
		struct Hash {
			std::size_t operator()(const Cell& cell) const {
				std::size_t h1 = std::hash<int>{}(cell.x);
				std::size_t h2 = std::hash<int>{}(cell.y);
				return h1 ^ (h2 << 1);
			}
		};
		struct Equal {
			bool operator()(const Cell& cell1,const Cell& cell2) const {
				return cell1.x == cell2.x && cell1.y == cell2.y;
			}
		};
	};
private:
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
	std::unordered_set<Cell,Cell::Hash,Cell::Equal> alive_cells;
	std::unordered_set<Cell,Cell::Hash,Cell::Equal> next_alive_cells;
	std::unordered_map<Cell,int,Cell::Hash,Cell::Equal> marked_cells;
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