#include "PushDownAutomaton.h"

PushDownAutomaton::PushDownAutomaton(std::vector<Transition> transitions, std::vector<uint16_t> acceptedStates,
	std::vector<uint16_t> states, std::vector<char> inputAlphabet, std::vector<char> stackAlphabet,
	uint16_t starterState, char initialStackSymbol)
	:m_transitions(transitions), m_acceptingStates(acceptedStates), m_states(states),m_inputAlphabet(inputAlphabet),
	m_stackAlphabet(stackAlphabet), m_startState(starterState), m_initialStackSymbol(initialStackSymbol)
{
}

void PushDownAutomaton::ClearAutomaton()
{
	m_transitions.clear();
	m_acceptingStates.clear();
	m_inputAlphabet.clear();
	m_stackAlphabet.clear();
	m_states.clear();
}

void PushDownAutomaton::PrintAutomaton()
{
	std::cout << "The Push-Down Automaton M = (";
	std::cout << "{";
	for (unsigned int i = 0; i < m_acceptingStates.size(); i++)
	{
		std::cout << "q" << std::to_string(m_acceptingStates[i]);
		if (i < m_acceptingStates.size() - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "}, {";
	for (unsigned int i = 0; i < m_inputAlphabet.size(); i++)
	{
		std::cout << m_inputAlphabet[i];
		if (i < m_inputAlphabet.size() - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "}, {";
	for (unsigned int i = 0; i < m_stackAlphabet.size(); i++)
	{
		std::cout << m_stackAlphabet[i];
		if (i < m_stackAlphabet.size() - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "}, Lambda,";
	std::cout << std::to_string(m_startState);
	std::cout << ", ";
	std::cout << m_initialStackSymbol;
	std::cout << ", {";
	for (unsigned int i = 0; i < m_acceptingStates.size(); i++)
	{
		std::cout << std::to_string(m_acceptingStates[i]);
		if (i < m_acceptingStates.size() - 1)
		{
			std::cout << ", ";
		}
	}
	std::cout << "})";
	std::cout << std::endl;
	std::cout << "With the Lambda transitions : ";
	std::cout << std::endl;
	for (unsigned int i = 0; i < m_transitions.size(); i++)
	{
		std::cout << "(" << m_transitions[i].m_currentState << ", " << m_transitions[i].m_inputSymbol << ", ";
		std::cout << m_transitions[i].m_stackSymbol << ", " << m_transitions[i].m_nextState << ", ";
		std::cout << m_transitions[i].m_nextStackSymbols << ")";
		if (i < m_transitions.size() - 1)
		{
			std::cout << ", ";
		}
		std::cout << std::endl;
	}
}

PushDownAutomaton PushDownAutomaton::ConvertFromGreibach(Grammar grammar)
{
	PushDownAutomaton pushDownAutomaton;
	std::vector<Transition> transitions;
	std::vector<uint16_t> acceptedStates;
	std::vector<uint16_t> states;
	std::vector<char> inputAlphabet;
	std::vector<char> stackAlphabet;
	uint16_t starterState = 0;
	char initialStackSymbol = grammar.GetStartSymbol();


	for (const auto& production : grammar.GetProductions())
	{

		auto current_state = starterState;
		auto next_state = starterState;
		char stack_symbol = production.first;
		char input_symbol = production.second[0];
		std::string next_stackSymbols = production.second.substr(1);
		if (next_stackSymbols.empty())
		{
			next_stackSymbols = "!";
		}
		Transition transition;
		transition.m_currentState = starterState;
		transition.m_nextState = starterState;
		transition.m_stackSymbol = production.first;
		transition.m_inputSymbol = production.second[0];
		transition.m_nextStackSymbols = next_stackSymbols;

		transitions.push_back(transition);
	}

	states.push_back(starterState);
	inputAlphabet = grammar.GetTerminals();
	stackAlphabet = grammar.GetNonterminals();

	pushDownAutomaton.m_transitions = transitions;
	pushDownAutomaton.m_acceptingStates = acceptedStates;
	pushDownAutomaton.m_states = states;
	pushDownAutomaton.m_inputAlphabet = inputAlphabet;
	pushDownAutomaton.m_stackAlphabet = stackAlphabet;
	pushDownAutomaton.m_startState = starterState;
	pushDownAutomaton.m_initialStackSymbol = initialStackSymbol;



	return pushDownAutomaton;
}

bool PushDownAutomaton::IsDeterministic()
{
	for (unsigned int i = 0; i < m_transitions.size(); i++)
	{
		for (unsigned int j = 0; j < m_transitions.size(); i++)
		{
			if (i != j)
			{
				if (m_transitions[i].m_currentState == m_transitions[j].m_currentState &&
					m_transitions[i].m_inputSymbol == m_transitions[j].m_inputSymbol &&
					m_transitions[i].m_stackSymbol == m_transitions[j].m_stackSymbol)
				{
					if (m_transitions[i].m_nextState != m_transitions[j].m_nextState ||
						m_transitions[i].m_nextStackSymbols != m_transitions[j].m_nextStackSymbols)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

bool PushDownAutomaton::CheckWord(std::string inputBand)
{
	if (inputBand.empty() && m_acceptingStates.empty())
	{
		std::cerr << "(!)" << "Zero length input string not allowed, accepting states is not empty." << std::endl;
		return false;
	}

	for (auto& j : inputBand)
	{
		if (std::find(m_inputAlphabet.begin(), m_inputAlphabet.end(), j) == m_inputAlphabet.end())
		{
			std::cerr << "(!)" << "Character not found in allowed input alphabet." << std::endl;
			return false;
		}
	}

	uint16_t current = m_startState;
	std::stack<char> stack;
	stack.push(m_initialStackSymbol);

	for (auto& i : inputBand)
	{
		for (auto& j : m_transitions)
		{
			if (j.m_currentState == current && j.m_inputSymbol == i && j.m_stackSymbol == stack.top())
			{
				stack.pop();
				for (auto& k : j.m_nextStackSymbols)
				{
					stack.push(k);
				}
				current = j.m_nextState;
				break;
			}
		}

	}
	auto final = stack.top();

	return final == '!';
}

Transition& PushDownAutomaton::GetTransition(char inputSymbol, char stackSymbol, uint16_t currentState)
{
	auto it = std::find_if(m_transitions.begin(), m_transitions.end(),
		[=](const Transition& t) {
			return t.m_inputSymbol == inputSymbol
				&& t.m_stackSymbol == stackSymbol
				&& t.m_currentState == currentState;});

	if (it == m_transitions.end())
	{
		std::cerr << "(!)" << "Transition not found." << std::endl;
	}
	return *it;
}

void PushDownAutomaton::AddTransition(const Transition& transition)
{
	m_transitions.push_back(transition);
}

std::vector<Transition> PushDownAutomaton::GetTransition() const
{
	return m_transitions;
}

std::vector<uint16_t> PushDownAutomaton::GetAcceptedStates() const
{
	return m_acceptingStates;
}

std::vector<uint16_t> PushDownAutomaton::GetStates() const
{
	return m_states;
}

std::vector<char> PushDownAutomaton::GetInputAlphabet() const
{
	return m_inputAlphabet;
}

std::vector<char> PushDownAutomaton::GetStackAlphabet() const
{
	return m_stackAlphabet;
}

uint16_t PushDownAutomaton::GetStartState() const
{
	return m_startState;
}

char PushDownAutomaton::GetInitialStackSymbol() const
{
	return m_initialStackSymbol;
}

PushDownAutomaton::~PushDownAutomaton()
{
	ClearAutomaton();
}

std::ostream& operator<<(std::ostream& flux, PushDownAutomaton pushDownAutomaton)
{
	flux << "The Push-Down Automaton M = (";
	flux << "{";
	for (unsigned int i = 0; i < pushDownAutomaton.m_acceptingStates.size(); i++)
	{
		flux << "q" << std::to_string(pushDownAutomaton.m_acceptingStates[i]);
		if (i < pushDownAutomaton.m_acceptingStates.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "}, {";
	for (unsigned int i = 0; i < pushDownAutomaton.m_inputAlphabet.size(); i++)
	{
		flux << pushDownAutomaton.m_inputAlphabet[i];
		if (i < pushDownAutomaton.m_inputAlphabet.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "}, {";
	for (unsigned int i = 0; i < pushDownAutomaton.m_stackAlphabet.size(); i++)
	{
		flux << pushDownAutomaton.m_stackAlphabet[i];
		if (i < pushDownAutomaton.m_stackAlphabet.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "}, Lambda,";
	flux << std::to_string(pushDownAutomaton.m_startState);
	flux << ", ";
	flux << pushDownAutomaton.m_initialStackSymbol;
	flux << ", {";
	for (unsigned int i = 0; i < pushDownAutomaton.m_acceptingStates.size(); i++)
	{
		flux << std::to_string(pushDownAutomaton.m_acceptingStates[i]);
		if (i < pushDownAutomaton.m_acceptingStates.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "})";
	flux << std::endl;
	flux << "With the Lambda transitions : ";
	flux << std::endl;
	for (unsigned int i = 0; i < pushDownAutomaton.m_transitions.size(); i++)
	{
		flux << "(" << pushDownAutomaton.m_transitions[i].m_currentState << ", " << pushDownAutomaton.m_transitions[i].m_inputSymbol << ", ";
		flux << pushDownAutomaton.m_transitions[i].m_stackSymbol << ", " << pushDownAutomaton.m_transitions[i].m_nextState << ", ";
		flux << pushDownAutomaton.m_transitions[i].m_nextStackSymbols << ")";
		if (i < pushDownAutomaton.m_transitions.size() - 1)
		{
			flux << ", ";
		}
		flux << std::endl;
	}
	return flux;
}
