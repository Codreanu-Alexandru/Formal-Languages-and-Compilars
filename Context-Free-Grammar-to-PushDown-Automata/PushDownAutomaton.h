#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <stack>
#include "Grammar.h"

struct Transition
{
public:
	uint16_t m_nextState;
	uint16_t m_currentState;
	char m_stackSymbol;
	char m_inputSymbol;
	std::string m_nextStackSymbols;
};

class PushDownAutomaton
{
public:
	/*Constructor*/
	PushDownAutomaton() = default;
	PushDownAutomaton(std::vector<Transition> transitions, std::vector<uint16_t> acceptedStates,
		std::vector<uint16_t> states, std::vector<char> inputAlphabet, std::vector<char> stackAlphabet,
		uint16_t starterState, char initialStackSymbol);

	/*Public operators*/
	friend std::ostream& operator<<(std::ostream& flux, PushDownAutomaton pushDownAutomaton);

	/*Public functions*/
	void AddTransition(const Transition& transition);
	void ClearAutomaton();
	void PrintAutomaton();
	PushDownAutomaton ConvertFromGreibach(Grammar grammar);

	/*Public check functions*/
	bool CheckWord(std::string inputBand);
	bool IsDeterministic();

	/*Getters*/
	std::vector<uint16_t> GetAcceptedStates() const;
	std::vector<Transition> GetTransition() const;
	std::vector<char> GetInputAlphabet() const;
	std::vector<char>GetStackAlphabet() const;
	std::vector<uint16_t> GetStates() const;
	char GetInitialStackSymbol() const;
	uint16_t GetStartState() const;

	/*Destructor*/
	~PushDownAutomaton();

private:
	/*Private members*/
	std::vector<uint16_t> m_acceptingStates;
	std::vector<Transition> m_transitions;
	std::vector<char> m_inputAlphabet;
	std::vector<char> m_stackAlphabet;
	std::vector<uint16_t> m_states;
	char m_initialStackSymbol;
	uint16_t m_startState;

	/*Private functions*/
	Transition& GetTransition(char inputSymbol, char stackSymbol, uint16_t currentState);
};

