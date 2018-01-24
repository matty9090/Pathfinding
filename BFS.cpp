#include "BFS.hpp"

#include <iostream>

using namespace std;

BFS::BFS(Tree &tree) : SearchAlgorithm(tree) {

}

void BFS::start(Tree::Node _start, Tree::Node _goal) {
	startn = _start;
	goaln = _goal;

	mNumSearches = 0;

	goal_found = false;

	open.insert(startn);

	cout << "Starting at (" << startn->pos.x << ", " << startn->pos.y << ")\n";
	cout << "Finding (" << goaln->pos.x << ", " << goaln->pos.y << ")\n\n";
}

int BFS::step() {
	if (!open.empty()) {
		mNumSearches++;

		Tree::Node current = *open.begin();
		open.erase(current);

		if (current == goaln) {
			goal_found = true;
			construct_path();
			return Found;
		}

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

			if (open.find(node) == open.end()) {
				data[node] = current;
				open.insert(node);
			}
		}

		closed.insert(current);
	} else
		return Failed;
	
	return Searching;
}

void BFS::construct_path() {
	if (goal_found) {
		Tree::Node n = goaln;
		path.push_back(n->pos);

		while (n = data[n]) {
			path.push_back(n->pos);
			cout << n->pos.toString() << endl;
		}

		reverse(path.begin(), path.end());
	} else
		cout << "Could not find path\n\n";
}
