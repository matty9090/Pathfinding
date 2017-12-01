#include "Tree.hpp"

#include <deque>
#include <set>
#include <map>
#include <iostream>

using namespace std;

Tree::Tree(Vec2<size_t> dims, std::vector<std::vector<Node>> &_grid) : w(dims.x), h(dims.y), grid(_grid) {
	
}

Tree::Node Tree::setNode(unsigned x, unsigned y, Vec2<> pos, int cost) {
	Node n = make_shared<_Node>(pos, cost);
	grid[y][x] = n;

	return n;
}

Tree::Node Tree::getNode(unsigned x, unsigned y) {
	return grid[y][x];
}

Tree::Node Tree::findNode(Vec2<> pos) {
	for (auto rows : grid) {
		for (auto node : rows) {
			if (node->pos.x == pos.x && node->pos.y == pos.y)
				return node;
		}
	}

	return nullptr;
}

std::list<Vec2<>> Tree::pathfind_bfs(Node start, Node goal) {
	list<Vec2<>> path;

	std::vector<Node> open;
	std::set<Node> closed;
	std::map<Node, Node> data;

	open.push_back(start);

	cout << "Starting at (" << start->pos.x << ", " << start->pos.y << ")\n";
	cout << "Finding (" << goal->pos.x << ", " << goal->pos.y << ")\n\n";

	bool goal_found = false;

	while (!open.empty()) {
		Node current = open.front();
		open.erase(open.begin(), open.begin() + 1);

		//cout << "Current node: " << current->pos.toString() << "\n";

		if (current == goal) {
			goal_found = true;
			break;
		}

		std::vector<Node> next;

		next.push_back(findNode(Vec2<>(current->pos.x + 1, current->pos.y)));
		next.push_back(findNode(Vec2<>(current->pos.x - 1, current->pos.y)));
		next.push_back(findNode(Vec2<>(current->pos.x, current->pos.y + 1)));
		next.push_back(findNode(Vec2<>(current->pos.x, current->pos.y + 1)));

		for (auto node : next) {
			if (node == nullptr || node->cost <= 0)
				continue;

			if (closed.find(node) != closed.end())
				continue;

			bool found = false;

			for (auto i : open)
				if (i == node)
					found = true;

			if (!found) {
				data[node] = current;
				open.push_back(node);
			}
		}

		closed.insert(current);
	}

	if (goal_found) {
		Node n = goal;
		path.push_back(n->pos);

		while (n = data[n])
			path.push_back(n->pos);

		path.reverse();
	} else
		cout << "Could not find path :(" << endl;

	for (auto i = path.begin(); i != path.end(); ++i) {
		std::cout << "(" << i->x << ", " << i->y << ")\n";
	}

	return path;
}
