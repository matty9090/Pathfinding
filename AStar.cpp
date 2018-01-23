#include "AStar.hpp"

#include <iostream>

using namespace std;

AStar::AStar(Tree &tree) : SearchAlgorithm(tree) {

}

void AStar::start(Tree::Node _start, Tree::Node _goal) {
	startn = _start;
	goaln = _goal;

	for (unsigned y = 0; y < tree.getGridSize().y; y++) {
		for (unsigned x = 0; x < tree.getGridSize().x; x++) {
			tree.getNode(x, y)->score = 10000;
			tree.getNode(x, y)->estimate = 10000;
		}
	}

	startn->score = 0;
	startn->estimate = heuristic(startn, goaln);

	open.insert(startn);

	goal_found = false;

	cout << "Starting at (" << startn->pos.x << ", " << startn->pos.y << ")\n";
	cout << "Finding (" << goaln->pos.x << ", " << goaln->pos.y << ")\n\n";
}

int AStar::step() {
	if (!open.empty()) {
		Tree::Node current;
		int min_score = 10000;

		for (auto n : open)
			if (n->estimate < min_score)
				min_score = n->estimate, current = n;

		if (current == goaln) {
			goal_found = true;
			construct_path();
			return Found;
		}

		open.erase(current);
		closed.insert(current);

		std::vector<Tree::Node> next;

		next.push_back(tree.findNode(Vec2<>(current->pos.x + 1, current->pos.y)));
		next.push_back(tree.findNode(Vec2<>(current->pos.x - 1, current->pos.y)));
		next.push_back(tree.findNode(Vec2<>(current->pos.x, current->pos.y + 1)));
		next.push_back(tree.findNode(Vec2<>(current->pos.x, current->pos.y - 1)));
		
		if (useDiag) {
			next.push_back(tree.findNode(Vec2<>(current->pos.x + 1, current->pos.y + 1)));
			next.push_back(tree.findNode(Vec2<>(current->pos.x - 1, current->pos.y - 1)));
			next.push_back(tree.findNode(Vec2<>(current->pos.x - 1, current->pos.y + 1)));
			next.push_back(tree.findNode(Vec2<>(current->pos.x + 1, current->pos.y - 1)));
		}

		for (auto node : next) {
			if (node == nullptr || node->cost <= 0 || closed.find(node) != closed.end())
				continue;

			if (open.find(node) == open.end())
				open.insert(node);

			int n_score = current->score + current->cost;

			if (n_score >= node->score)
				continue;

			data[node] = current;
			node->score = n_score;
			node->estimate = n_score + heuristic(node, goaln);
		}

		
	} else
		return Failed;
	
	return Searching;
}

void AStar::construct_path() {
	if (goal_found) {
		Tree::Node n = goaln;
		path.push_back(n->pos);

		while (n = data[n])
			path.push_back(n->pos);

		reverse(path.begin(), path.end());
	} else
		cout << "Could not find path :(\n\n";
}

// Manhattan Distance
int AStar::heuristic(Tree::Node start, Tree::Node end) {
	return abs(start->pos.x - end->pos.x) + abs(start->pos.y - end->pos.y);
}
