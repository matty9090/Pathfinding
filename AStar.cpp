#include "AStar.hpp"

#include <iostream>

using namespace std;

AStar::AStar(Tree &tree) : SearchAlgorithm(tree) {

}

void AStar::start(Tree::Node _start, Tree::Node _goal) {
	startn = _start;
	goaln = _goal;

	g_score[startn] = 0.f;
	f_score[startn] = heuristic(startn, goaln);

	open.insert(startn);

	bool goal_found = false;

	cout << "Starting at (" << startn->pos.x << ", " << startn->pos.y << ")\n";
	cout << "Finding (" << goaln->pos.x << ", " << goaln->pos.y << ")\n\n";
}

int AStar::step() {
	bool goal_found = false;

	if (!open.empty()) {
		Tree::Node current;
		float min_score = 10000.f;

		for (auto n : open)
			if (f_score[n] < min_score)
				min_score = f_score[n], current = n;

		if (current == goaln) {
			goal_found = true;
			return Found;
		}

		open.erase(current);
		closed.insert(current);

		std::vector<Tree::Node> next;

		next.push_back(getTree().findNode(Vec2<>(current->pos.x + 1, current->pos.y)));
		next.push_back(getTree().findNode(Vec2<>(current->pos.x - 1, current->pos.y)));
		next.push_back(getTree().findNode(Vec2<>(current->pos.x, current->pos.y + 1)));
		next.push_back(getTree().findNode(Vec2<>(current->pos.x, current->pos.y - 1)));

		for (auto node : next) {
			if (node == nullptr || node->cost <= 0 || closed.find(node) != closed.end())
				continue;

			if (open.find(node) == open.end())
				open.insert(node);

			float n_score = g_score[current] + (float)current->cost;

			if (g_score.find(node) == g_score.end())
				g_score[node] = 10000.f;

			if (n_score >= g_score[node])
				continue;

			data[node] = current;
			g_score[node] = n_score;
			f_score[node] = n_score + heuristic(node, goaln);
		}
	} else
		return Failed;
	
	return Searching;
}

// Manhattan Distance
float AStar::heuristic(Tree::Node start, Tree::Node end) {
	return (float)abs(start->pos.x - end->pos.x) + (float)abs(start->pos.y - end->pos.y);
}
