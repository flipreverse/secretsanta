#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include "argparse.hpp"
#include "wichtel.h"

#define CSV_DELIMITER ';'
#define CONFLICT_DELIMITER ','
#define MAX_RETRY 1000

using namespace std;

set<Wichtel> wichtelSet;
map<string,int> stats;
bool *visited;
bool **adjacent;
bool benchmark = false;
bool debug = false;
int maxIter = 1;

static bool parseInput(set<Wichtel> &wichtel, int argc, const char *argv[]) {
	ArgumentParser parser;
	int val;

	parser.addArgument("--benchmark", 1);
	parser.addArgument("--debug");
	parser.addFinalArgument("fname");
	parser.parse(argc, argv);
	// Retrieve file name from cmdline
	string fname = parser.retrieve<string>("fname");
	if (parser.retrieve<string>("benchmark").size() > 0) {
		val = std::stoi(parser.retrieve<string>("benchmark"));
		if (val > 0) {
			benchmark = true;
			maxIter = val;
		} else {
			cerr << parser.usage() << endl;
			exit(EXIT_FAILURE);
		}
	}
	debug = parser.exists("debug");
	
	string line;
	// Open 'fname' for reading
	ifstream ifs(fname, ifstream::in);
	if (!ifs.is_open()) {
		cerr << "Cannot open '" << fname << "' for reading!" << endl;
		return false;
	}
	// Read content line by line
	while (getline(ifs, line)) {
		stringstream ss(line);
		vector<string> tokens;
		string temp;
		// Tokenize each line by comma
		while (getline(ss, temp, CSV_DELIMITER)) {
			tokens.push_back(temp);
		}
		// Do not expect more than two tokens!
		if (tokens.size() < 2) {
			cerr << "Argument contains to many tokens: " << line << endl;
			continue;
		}
		pair<set<Wichtel>::iterator,bool> ret = wichtel.emplace(tokens[0], tokens[1]);
		if (!ret.second) {
			cerr << "Duplicate Wichtel found in input: " << line << endl;
			continue;
		}
	}

	return true;
}

static void printWichtel(set<Wichtel> wichtelVector) {
	cout << "----------" << endl;
	for (auto & wichtel : wichtelVector) {
		cout << wichtel << endl;
	}
	cout << "----------" << endl;
}

static bool calcSol(bool *visited, bool **adjacent, unsigned int size, vector<int> &path) {
	int node, next_node, i = 0;

	// Randomly choose a start node
	node = std::rand() % size;
	do {
		// Randomly choose the next node
		next_node = std::rand() % size;
		// Have we already visited this node?
		// Is there an edge between node and next_node?
		if (!visited[next_node] && adjacent[node][next_node]) {
			visited[next_node] = true;
			path.push_back(next_node);
			node = next_node;
		} else {
			i++;
		}
	} while (path.size() < size && i < MAX_RETRY);
	if (i == MAX_RETRY) {
		return false;
	} else {
		// Make it a full cycle
		path.push_back(path.front());
		return true;
	}
}

static string pathToString(vector<int> &nodes) {
	string ret = "";

	for (unsigned int i = 0; i < nodes.size(); i++) {
		ret.append(std::to_string(nodes[i]));
		if (i < (nodes.size() - 1)) {
			ret.append(",");
		}
	}

	return ret;
}

static void printSol(set<Wichtel> &wichtelSet, vector<int> &path) {
	vector<Wichtel> wichtelArr(wichtelSet.begin(), wichtelSet.end());

	if (debug) {
		cout << "Path: " << pathToString(path) << endl;
	}
	for (unsigned int i = 0; i < (path.size() - 1); i++) {
		if (debug) {
			cout << wichtelArr[path[i]].getName() << "(" << path[i] << ")";
			cout << "-->";
			cout  << wichtelArr[path[i + 1]].getName() << "(" << path[i + 1] << ")" << endl;
		} else {
			cout << wichtelArr[path[i]].getName() << "-->" << wichtelArr[path[i + 1]].getName() << endl;
		}
	}
}

static void reset(bool *visited, unsigned int size) {
	for (unsigned int i = 0; i < size; i++) {
		visited[i] = false;
	}
}

static void initGraph(bool *visited, bool **adjacent, unsigned int size) {
	for (unsigned int i = 0; i < size; i++) {
		adjacent[i] = new bool[wichtelSet.size()];
		for (unsigned int j = 0; j < size; j++) {
			if (i == j) {
				adjacent[i][j] = false;
			} else {
				adjacent[i][j] = true;
			}
		}
	}
	reset(visited, size);
}

int main(int argc, const char *argv[]) {
	vector<int> solWichtel;

	if (!parseInput(wichtelSet, argc, argv)) {
		return EXIT_FAILURE;
	}

	std::srand(unsigned(std::time(0)));
	visited = new bool[wichtelSet.size()];
	adjacent = new bool*[wichtelSet.size()];
	initGraph(visited, adjacent, wichtelSet.size());

	vector<int> path;
	for (int i = 0; i < maxIter; i++) {
		path.clear();;
		reset(visited, wichtelSet.size());
		if (!calcSol(visited, adjacent, wichtelSet.size(), path)) {
			cerr << "Cannot find solution!" << endl;
			return EXIT_FAILURE;
		}
		string pathString = pathToString(path);
		if (benchmark) {
			auto ret = stats.emplace(pathString, 0);
			ret.first->second++;
		}
	}

	if (benchmark) {
		for (auto x : stats) {
			cout << "[" << x.first << ":" << x.second << "]" << endl;
		}
	} else {
		printWichtel(wichtelSet);
		printSol(wichtelSet, path);
	}

	delete visited;
	for (unsigned int i = 0; i < wichtelSet.size(); i++) {
		delete adjacent[i];
	}
	delete adjacent;

	return EXIT_SUCCESS;
}
