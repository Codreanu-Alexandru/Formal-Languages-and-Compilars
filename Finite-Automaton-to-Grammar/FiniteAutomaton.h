#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>

struct Tranzition
{
public:
	/*Public Members*/
	std::string m_state;
	char m_symbol;
	std::string m_newState;

	/*Public operator*/
	friend std::ostream& operator<<(std::ostream& flux, const Tranzition& tranzition);
};

class FiniteAutomaton
{
public:
	/*Public constructors*/
	FiniteAutomaton(bool readFromFile = true);
	FiniteAutomaton(std::vector<std::string> state, std::vector<char> alphabet,
	std::string initialState, std::vector<std::string> finalState, std::vector<Tranzition> transition);

	/*Public operator*/
	friend std::ostream& operator<< (std::ostream& flux, const FiniteAutomaton& finiteAutomaton);

	/*Public check funtions*/
	bool CheckAutomatonValidity();
	bool CheckAutomatonInputFile();
	bool IsDeterministic();
	bool CheckWord(std::string word);

private:

	/*Private members*/
	std::vector<std::string> m_state;
	std::vector<char> m_alphabet;
	std::string m_initialState;
	std::vector<std::string> m_finalState;
	std::vector<Tranzition> m_transition;

};

