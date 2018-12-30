#ifndef __WICHTEL_H__
#define __WICHTEL_H__

#include <vector>

class Wichtel {
private:
	int id;
	std::string name;
	std::string email;
	std::vector<std::string> conflicts;

public:
	Wichtel (void) {

	}
	Wichtel(int id, std::string name, std::string email) {
		this->id = id;
		this->name = name;
		this->email = email;
	}
	int getId(void) const { return this->id; }
	std::string getName(void) const { return this->name; }
	std::string getEMail(void) const { return this->email; }
	void setName(std::string value) { this->name = value; }
	void setEMail(std::string value) { this->email = value; }
	void addConflict(std::string value) { this->conflicts.push_back(value); }
	std::vector<std::string> getConflicts(void) const { return this->conflicts; }
	std::string toString(void) const {
		std::string ret;

		ret.append("Wichtel:\tID=" + std::to_string(this->getId()) + ", Name=" + this->getName() + ", E-Mail=" + this->getEMail());
		if (!this->conflicts.empty()) {
			ret.append(", Conflicts=");
			for (unsigned int i = 0; i < this->conflicts.size(); i++) {
				ret.append(this->conflicts[i]);
				if (i < (this->conflicts.size() - 1)) {
					ret.append(",");
				}
			}
		}
		return ret;
	}

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
#endif // __WICHTEL_H__
