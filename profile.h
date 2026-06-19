#pragma once
#include <string>
class Profile
{
private:
	std::string m_name;

public:
	Profile() {
		m_name = "!";
	}
	
	Profile(std::string input_name) {
		m_name = input_name;
	}

	std::string get_name() const;
};

