#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include "argparse.hpp"

using namespace std;

class Wichtel {
private:
	std::string name;
	std::string email;

public:
	Wichtel (void) {

	}
	Wichtel(std::string name, std::string email) {
		this->name = name;
		this->email = email;
	}
	std::string getName(void) const { return this->name; }
	std::string getEMail(void) const { return this->email; }
	void setName(std::string value) { this->name = value; }
	void setEMail(std::string value) { this->email = value; }
	std::string toString(void) const { return "Wichtel:\tName=" + this->getName() + ", E-Mail=" + this->getEMail(); }

	bool operator==(const Wichtel &rhs) const {
		return this->getName().compare(rhs.getName()) == 0;
	}
	bool operator!=(const Wichtel &rhs) const { return !(*this == rhs); }
	bool operator<(const Wichtel &rhs) const {
		return this->getName().compare(rhs.getName()) < 0;
	}
	/* "Relational Operators" - https://en.cppreference.com/w/cpp/language/operators */
	bool operator>(const Wichtel &rhs)  const { return rhs < *this; }
	bool operator<=(const Wichtel &rhs) const { return !(*this > rhs); }
	bool operator>=(const Wichtel &rhs) const { return !(*this < rhs); }
};

std::ostream& operator<<(std::ostream &out, const Wichtel &wichtel) {
	return out << wichtel.toString();
}

static bool parseInput(vector<Wichtel> &wichtel, int argc, const char *argv[]) {
	ArgumentParser parser;
	set<Wichtel> uniqueWichtel;

	parser.addFinalArgument("fname");
	parser.parse(argc, argv);
	// Retrieve file name from cmdline
	string fname = parser.retrieve<string>("fname");
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
		while (getline(ss, temp, ',')) {
			tokens.push_back(temp);
		}
		// Do not expect more than two tokens!
		if (tokens.size() != 2) {
			cerr << "Argument contains to many tokens: " << line << endl;
			continue;
		}
		auto ret = uniqueWichtel.emplace(tokens[0], tokens[1]);
		if (!ret.second) {
			cerr << "Duplicate Wichtel found in input: " << line << endl;
			continue;	
		}
#if 0
		// Create a new Wichtel for each line
		wichtel.push_back(Wichtel());
		Wichtel &newWichtel = wichtel.back();
		// Set a Wichtel's name and email to the values parsed above
		newWichtel.setName(tokens[0]);
		newWichtel.setEMail(tokens[1]);
#endif
	}

	wichtel.assign(uniqueWichtel.begin(), uniqueWichtel.end());
	return true;
}

static bool validSol(vector<Wichtel> &wichtel, vector<Wichtel> &sol) {
	for (std::vector<Wichtel>::size_type i = 0; i < wichtel.size(); i++) {
		if (wichtel[i] == sol[i]) {
#ifdef DEBUG
			cerr << "i=" << i << endl;
#endif
			return false;
		}
		for (std::vector<Wichtel>::size_type k = 0; k < wichtel.size(); k++) {
			if (sol[i] == wichtel[k]) {
				if (sol[k] == wichtel[i]) {
#if 0
					cout << "Detected circle. i=" << i << ", k=" << k << endl;
#endif
					return false;
				}
			}
		}
	}
	return true;
}

static void printWichtel(std::string prefix, vector<Wichtel> wichtelVector) {
	cout << prefix << endl;
	cout << "----------" << endl;
	for (auto & wichtel : wichtelVector) {
		cout << wichtel << endl;
	}
	cout << "----------" << endl;
}

static int myrandom(int i) { return std::rand() % i; }

static void calcSol(vector<Wichtel> &solWichtel) {
	std::srand(unsigned(std::time(0)));
	random_shuffle(solWichtel.begin(), solWichtel.end(), myrandom);
}

vector<Wichtel> inputWichtel;

int main(int argc, const char *argv[]) {
	vector<Wichtel> solWichtel;
	int n = 0;

	if (!parseInput(inputWichtel, argc, argv)) {
		return EXIT_FAILURE;
	}


	do {
		solWichtel = inputWichtel;
		calcSol(solWichtel);
		n++;
	} while(!validSol(inputWichtel, solWichtel));
	cout << "Valid solution found in " << n << " iterations." << endl;

	printWichtel("input", inputWichtel);
	printWichtel("sol", solWichtel);

	return EXIT_SUCCESS;
}
