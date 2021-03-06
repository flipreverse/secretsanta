#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <cstring>
#include "argparse.hpp"
#include "wichtel.h"
#include "quickmail.h"

#define CSV_DELIMITER ';'
#define CONFLICT_DELIMITER ','
#define MAIL_DELIMITER ','
#define MAX_RETRY 1000
#define BODY_FMT_STRING "Hallo %s,\n\n"\
			"der Wichtel-O-Mat hat Dir %s als Deinen Wichtel bestimmt.\n\n"\
			"Viele Grüße\n"\
			"Dein Wichtel-O-Mat\n"\
			"      *\n"\
			"     /.\\\n"\
			"    /..'\\\n"\
			"    /'.'\\\n"\
			"   /.''.'\\\n"\
			"   /.'.'.\\\n"\
			"  /'.''.'.\\\n"\
			"  ^^^[_]^^^"
#define BODY_MAX_LEN 300

using namespace std;

map<string,Wichtel> wichtelMap;
map<string,int> stats;
bool *visited;
bool **adjacent;
bool benchmark = false;
bool debug = false;
bool ignoreConflicts = false;
bool sendMail = false;
string mailSubject = "Nachricht vom Wichtel-O-Mat";
string mailSender;
string mailServer;
string mailUser;
string mailPass;
int mailPort = 25;
int maxIter = 1;
int IDs = 0;

static bool parseInput(map<string,Wichtel> &wichtel, int argc, const char *argv[], int &ids) {
	ArgumentParser parser;
	int val;

	parser.addArgument("--benchmark", 1);
	parser.addArgument("--debug");
	parser.addArgument("--ignore-conflicts");
	parser.addArgument("--send-email", 1);
	parser.addArgument("--email-subject", 1);
	parser.addArgument("--email-server", 1);
	parser.addArgument("--email-port", 1);
	parser.addArgument("--email-user", 1);
	parser.addArgument("--email-pass", 1);
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
	debug = parser.retrieve<string>("debug") == "true" ;
	ignoreConflicts = parser.retrieve<string>("ignore-conflicts") == "true";
	if (parser.retrieve<string>("send-email").size() > 0) {
		sendMail = true;
		mailSender = parser.retrieve<string>("send-email");
	}
	if (parser.retrieve<string>("email-subject").size() > 0) {
		mailSubject = parser.retrieve<string>("email-subject");
	}
	if (parser.retrieve<string>("email-server").size() > 0) {
		mailServer = parser.retrieve<string>("email-server");
	}
	if (parser.retrieve<string>("email-port").size() > 0) {
		mailPort = std::stoi(parser.retrieve<string>("email-port"));
	}
	if (parser.retrieve<string>("email-user").size() > 0) {
		mailUser = parser.retrieve<string>("email-user");
	}
	if (parser.retrieve<string>("email-pass").size() > 0) {
		mailPass = parser.retrieve<string>("email-pass");
	}
	if (sendMail && (mailServer.length() == 0 || mailPort <= 0)) {
		cerr << parser.usage() << endl;
		exit(EXIT_FAILURE);
	}

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
		vector<string> emails;
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
		ss.clear(); ss.str("");
		ss << tokens[1];
		while (getline(ss, temp, MAIL_DELIMITER)) {
			emails.push_back(temp);
		}
		auto ret = wichtel.emplace(tokens[0], Wichtel(ids, tokens[0], emails));
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
		} else if (visited[next_node] && adjacent[node][next_node] && 
			   next_node == path.front() && path.size() == size) {
			// We have visited next_node *and* there is an edge between node and next_node.
			// Moreover, next_node corresponds to the first node on the path, and
			// we've already visited all other nodes.
			path.push_back(next_node);
			break;
		} else {
			i++;
		}
	} while (i < MAX_RETRY);
	if (i == MAX_RETRY) {
		// Fill the remaining gaps with dummy values (-1)
		// Add one more because a full cycle contains #nodes+1 nodes
		for (unsigned int j =  path.size(); j < (size + 1); j++) {
			path.push_back(-1);
		}
		return false;
	} else {
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

static void printSol(vector<Wichtel> &wichtelArr, vector<int> &path) {
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

static void notifyWichtel(vector<Wichtel> &wichtelArr, vector<int> &path, string subject) {
	const char* errmsg;
	char *body;
	int len;

	quickmail_initialize();
	body = (char*)malloc(BODY_MAX_LEN);
	if (body == NULL) {
		cout << "Cannot allocate memory for email body!" << endl;
		exit(EXIT_FAILURE);
	}

	for (unsigned int i = 0; i < (path.size() - 1); i++) {
		Wichtel &srcWichtel = wichtelArr[path[i]];
		Wichtel &dstWichtel = wichtelArr[path[i + 1]];
		if (debug) {
			cout << "====================" << endl;
		}
		quickmail mailobj = quickmail_create(mailSender.c_str(), subject.c_str());
		for (auto email : srcWichtel.getEMails()) {
			quickmail_add_to(mailobj, email.c_str());
		}
		quickmail_add_header(mailobj, "Importance: Low");
		quickmail_add_header(mailobj, "X-Priority: 5");
		quickmail_add_header(mailobj, "X-MSMail-Priority: Low");
		//quickmail_set_body(mailobj, "This is a test e-mail.\nThis mail was sent using libquickmail.");
		len = snprintf(body, BODY_MAX_LEN, BODY_FMT_STRING, srcWichtel.getName().c_str(), dstWichtel.getName().c_str());
		quickmail_add_body_memory(mailobj, "text/plain; charset=UTF-8", body, len, 0);
		if (debug) {
			quickmail_fsave(mailobj, stdout);
			quickmail_set_debug_log(mailobj, stdout);
		}
		errmsg = quickmail_send(mailobj, mailServer.c_str(), mailPort, (mailUser.length() > 0 ? mailUser.c_str() : NULL), (mailPass.length() > 0 ? mailPass.c_str() : NULL));
		if (errmsg != NULL) {
			cout << "Error sending e-mail to ";
		} else {
			cout << "Successfully sent an e-mail to ";
		}
		cout  << srcWichtel.getName() << "(";
		for (unsigned int i = 0; i < srcWichtel.getEMails().size(); i++) {
			cout << srcWichtel.getEMails()[i];
			if (i < (srcWichtel.getEMails().size() - 1)) {
				cout << ",";
			}
		}
		cout << "). " << endl;

		quickmail_destroy(mailobj);
		if (debug) {
			cout << "====================" << endl;
		}
	}

	quickmail_cleanup();
	free(body);
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
		} else if (benchmark) {
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
		vector<Wichtel> wichtelArr;
		wichtelArr.resize(wichtelMap.size());
		std::for_each(wichtelMap.begin(), wichtelMap.end(), [&wichtelArr](const std::map<string,Wichtel>::value_type &p) { wichtelArr[p.second.getId()] = p.second; });

		if (sendMail) {
			notifyWichtel(wichtelArr, path, mailSubject);
		} else {
			printWichtel(wichtelMap);
			printSol(wichtelArr, path);
		}
	}

	delete visited;
	for (unsigned int i = 0; i < wichtelMap.size(); i++) {
		delete adjacent[i];
	}
	delete adjacent;

	return EXIT_SUCCESS;
}
