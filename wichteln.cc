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

map<string,Wichtel> wichtelMap;
map<string,int> stats;
bool *visited;
bool **adjacent;
bool benchmark = false;
bool debug = false;
bool ignoreConflicts = false;
int maxIter = 1;
int IDs = 0;

static bool parseInput(map<string,Wichtel> &wichtel, int argc, const char *argv[], int &ids) {
	ArgumentParser parser;
	int val;

	parser.addArgument("--benchmark", 1);
	parser.addArgument("--debug");
	parser.addArgument("--ignore-conflicts");
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
	ignoreConflicts = parser.exists("ignore-conflicts");	

	string line;
	// Open 'fname' for reading
	ifstream ifs(fname, ifstream::in);
	if (!ifs.is_open()) {
		cerr << "Cannot open '" << fname << "' for reading!" << endl;
		return false;
	}
	// Read content line by line
	int lineCounter = 0;
	while (getline(ifs, line)) {
		lineCounter++;
		stringstream ss(line);
		vector<string> tokens;
		string temp;
		// Tokenize each line by comma
		while (getline(ss, temp, CSV_DELIMITER)) {
			tokens.push_back(temp);
		}
		// Expecting two tokens at least!
		if (tokens.size() < 2) {
			cerr << "Argument contains to many tokens: " << line << endl;
			continue;
		}
		auto ret = wichtel.emplace(tokens[0], Wichtel(ids, tokens[0], tokens[1]));
		if (!ret.second) {
			cerr << "Duplicate Wichtel found at input line " << lineCounter << ": '" << line << "'" << endl;
			continue;
		}
		// Increment the id *only* if the Wichtel has been successfully inserted
		ids++;
		// Found column 'conflicts'
		if (tokens.size() > 2) {
			stringstream ss(tokens[2]);
			string temp;
			while (getline(ss, temp, CONFLICT_DELIMITER)) {
				ret.first->second.addConflict(temp);
			}
		}
	}

	return true;
}

static void printWichtel(map<string,Wichtel> wichtelVector) {
	cout << "----------" << endl;
	for (auto & elem : wichtelVector) {
		cout << elem.second << endl;
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

static void printSol(map<string,Wichtel> &wichtelMap, vector<int> &path) {
	vector<Wichtel> wichtelArr;(wichtelMap.begin(), wichtelMap.end());
	std::for_each(wichtelMap.begin(), wichtelMap.end(), [&wichtelArr](const std::map<string,Wichtel>::value_type &p) { wichtelArr.push_back(p.second); });

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
		adjacent[i] = new bool[wichtelMap.size()];
		for (unsigned int j = 0; j < size; j++) {
			if (i == j) {
				adjacent[i][j] = false;
			} else {
				adjacent[i][j] = true;
			}
		}
	}
	reset(visited, size);
	if (!ignoreConflicts) {
		for (auto &elem : wichtelMap) {
			for (auto conflict : elem.second.getConflicts()) {
				auto it = wichtelMap.find(conflict);
				if (it == wichtelMap.end()) {
					cout << "Conflicting Wichtel not found: '" << conflict << "'" << endl;
					continue;
				}
				adjacent[elem.second.getId()][it->second.getId()] = false;
				adjacent[it->second.getId()][elem.second.getId()] = false;
				if (debug) {
					cout << "Deleting edges between '" << elem.second.getName() << "' and '" << it->second.getName() << "'" << endl;
				}
			}
		}
	} else {
		cout << "Ignoring conflicts!" << endl;
	}
	if (debug) {
		cout << "Adjacency matrix:" << endl;
		cout << "  ";
		for (unsigned int i = 0; i < size; i++) {
			cout << i;
			if (i < (size - 1)) {
				cout << " ";
			}
		}
		cout << endl;
		for (unsigned int i = 0; i < (size * 2 + 2); i++) {
			cout << "-";
		}
		cout << endl;
		for (unsigned int i = 0; i < size; i++) {
			cout << i << "|";
			for (unsigned int j = 0; j < size; j++) {	
				cout << adjacent[i][j];
				if (j < (size - 1)) {
					cout << " ";
				}
			}
			cout << endl;
		}	
	}
}

int main(int argc, const char *argv[]) {
	int failed = 0;
	vector<int> path;

	if (!parseInput(wichtelMap, argc, argv, IDs)) {
		return EXIT_FAILURE;
	}

	std::srand(unsigned(std::time(0)));
	visited = new bool[wichtelMap.size()];
	adjacent = new bool*[wichtelMap.size()];
	initGraph(visited, adjacent, wichtelMap.size());

	for (int i = 0; i < maxIter; i++) {
		path.clear();;
		reset(visited, wichtelMap.size());
		if (!calcSol(visited, adjacent, wichtelMap.size(), path)) {
			if (!benchmark) {
				cerr << "Cannot find solution!" << endl;
				return EXIT_FAILURE;
			} else {
				failed++;
			}
		}
		if (benchmark) {
			auto ret = stats.emplace(pathToString(path), 0);
			ret.first->second++;
		}
	}

	if (benchmark) {
		cout << failed << " out of " << maxIter << " runs failed." << endl;
		for (auto x : stats) {
			cout << "[" << x.first << ":" << x.second << "]" << endl;
		}
	} else {
		printWichtel(wichtelMap);
		printSol(wichtelMap, path);
	}

	delete visited;
	for (unsigned int i = 0; i < wichtelMap.size(); i++) {
		delete adjacent[i];
	}
	delete adjacent;

	return EXIT_SUCCESS;
}
