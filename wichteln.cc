#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

class Wichtel {
private:
	std::string name;
	std::string email;

public:
	Wichtel(std::string name, std::string email) {
		this->name = name;
		this->email = email;
	}
	std::string getName(void) const { return this->name; }
	std::string getEMail(void) const { return this->email; }
	std::string toString(void) const { return "Name: " + this->getName() + ", E-Mail: " + this->getEMail(); }

	bool operator==(const Wichtel &rhs) {
		return this->getName().compare(rhs.getName()) == 0 &&
			this->getEMail().compare(rhs.getEMail()) == 0;
	}
	bool operator!=(const Wichtel &rhs) { return !(*this == rhs); }	
};

int main(int argc, const char *argv[]) {

	Wichtel a("alex", "foo@bar.de");
	Wichtel b("laura", "foo@bar.de");
	Wichtel c("laura", "foo@bar.de");


	cout << "a == b? " << (a == b) << endl;
	cout << "a == c? " << (a == c) << endl;
	cout << "c == b? " << (c == b) << endl;

	return EXIT_SUCCESS;
}
