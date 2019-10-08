#ifndef __WICHTEL_H__
#define __WICHTEL_H__

#include <vector>

class Wichtel {
private:
	int m_id;
	std::string m_name;
	std::vector<std::string> m_emails;
	std::vector<std::string> m_conflicts;

public:
	Wichtel (void) {

	}
	Wichtel(int m_id, std::string m_name, std::vector<std::string> m_emails) {
		this->m_id = m_id;
		this->m_name = m_name;
		this->m_emails = m_emails;
	}
	int getId(void) const { return this->m_id; }
	std::string getName(void) const { return this->m_name; }
	std::vector<std::string>& getEMails(void) { return this->m_emails; }
	void setName(std::string value) { this->m_name = value; }
	void addConflict(std::string value) { this->m_conflicts.push_back(value); }
	std::vector<std::string> getConflicts(void) const { return this->m_conflicts; }
	std::string toString(void) const {
		std::string ret;

		ret.append("Wichtel:\tID=" + std::to_string(this->getId()) + ", Name=" + this->getName());
		ret.append(", E-Mail-Adressen=");
		for (unsigned int i = 0; i < this->m_emails.size(); i++) {
			ret.append(this->m_emails[i]);
			if (i < (this->m_emails.size() - 1)) {
				ret.append(",");
			}
		}
		if (!this->m_conflicts.empty()) {
			ret.append(", Conflicts=");
			for (unsigned int i = 0; i < this->m_conflicts.size(); i++) {
				ret.append(this->m_conflicts[i]);
				if (i < (this->m_conflicts.size() - 1)) {
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
