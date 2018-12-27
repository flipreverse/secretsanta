#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
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

	bool operator==(const Wichtel &rhs) {
		return this->getName().compare(rhs.getName()) == 0 &&
			this->getEMail().compare(rhs.getEMail()) == 0;
	}
	bool operator!=(const Wichtel &rhs) { return !(*this == rhs); }	
};

std::ostream& operator<<(std::ostream &out, const Wichtel &wichtel) {
	return out << wichtel.toString();
}

vector<Wichtel> inputWichtel;

bool parseInput(int argc, const char *argv[]) {
	ArgumentParser parser;

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
		// Create a new Wichtel for each line
		inputWichtel.push_back(Wichtel());
		Wichtel &newWichtel = inputWichtel.back();
		// Set a Wichtel's name and email to the values parsed above
		newWichtel.setName(tokens[0]);
		newWichtel.setEMail(tokens[1]);
	}
	return true;
}

int main(int argc, const char *argv[]) {
	if (!parseInput(argc, argv)) {
		return EXIT_FAILURE;
	}

	for (auto & wichtel : inputWichtel) {
		cout << wichtel << endl;
	}

	return EXIT_SUCCESS;
}
