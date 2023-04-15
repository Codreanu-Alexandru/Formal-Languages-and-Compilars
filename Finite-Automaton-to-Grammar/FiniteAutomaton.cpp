#include "FiniteAutomaton.h"
static constexpr const char lambda = '$';

FiniteAutomaton::FiniteAutomaton(bool readFromFile)
{
	if (readFromFile)
	{
		if (!CheckAutomatonInputFile())
		{
			std::ifstream input("inputFiniteAutomaton.txt");
			if (!input.is_open())
			{
				std::cerr << "(!)" << "Unable to open file." << std::endl;
			}

			std::regex stateRegex("[_][A-Z]+[_]");
			std::string state;
			std::string in;

			while (!input.eof())
			{

				input >> in;

				if (std::regex_match(in, stateRegex))
				{
					state = in;
				}
				else
				{
					if (state == "_Q_")
					{
						m_state.push_back(in);
					}
					else if (state == "_SIGMA_")
					{
						m_alphabet.push_back(in[0]);
					}
					else if (state == "_START_")
					{
						m_initialState = in;
					}
					else if (state == "_FINAL_")
					{
						m_finalState.push_back(in);
					}
					else if (state == "_T_")
					{
						Tranzition t;
						t.m_state = in;
						input >> in;
						t.m_symbol = in[0];
						input >> in;
						t.m_newState = in;
						m_transition.push_back(t);
					}
				}
			}
			if (!CheckAutomatonValidity())
			{
				std::cerr << "(!)" << "CAUTION! AUTOMATON NOT VALID!" << std::endl;
			}
			input.close();
		}
		else
		{
			std::cerr << "(!)" << "AUTOMATON INPUT NOT VALID!" << std::endl;
		}

	}
}

FiniteAutomaton::FiniteAutomaton(std::vector<std::string> state, std::vector<char> alphabet,
	std::string initialState, std::vector<std::string> finalState, std::vector<Tranzition> transition)
	:m_state(state), m_alphabet(alphabet), m_initialState(initialState), m_finalState(finalState), m_transition(transition)
{
	if (!CheckAutomatonValidity())
	{
		std::cout << "(!)" << "CAUTION, AUTOMATON INVALID!" << std::endl;
	}
}

bool FiniteAutomaton::CheckAutomatonValidity()
{
	/*First check*/
	if (std::find(m_state.begin(), m_state.end(), m_initialState) == m_state.end())
	{
		std::cerr << "(!)" << "Start state was not found in state vector." << std::endl;
		return false;
	}

	/*Second check*/
	for (unsigned int i = 0; i < m_finalState.size(); i++)
	{
		if (std::find(m_state.begin(), m_state.end(), m_finalState[i]) == m_state.end())
		{
			std::cerr << "(!)" << "Final state was not found in state vector." << std::endl;
			return false;
		}
	}

	/*Third,Fourth & Fifth check*/
	bool initialStateCheck = false;
	bool finalStateCheck = false;
	for (const auto& i : m_transition)
	{
		if (std::find(m_alphabet.begin(), m_alphabet.end(), i.m_symbol) == m_alphabet.end())
		{
			if (i.m_symbol != lambda)
			{
				std::cerr << "(!)" << "Symbol was not found in alphabet vector." << std::endl;
				return false;
			}
		}
		if (std::find(m_state.begin(), m_state.end(), i.m_state) == m_state.end())
		{
			std::cerr << "(!)" << "State was not found in state vector." << std::endl;
			return false;
		}
		if (std::find(m_state.begin(), m_state.end(), i.m_newState) == m_state.end())
		{
			if (i.m_newState.find(lambda) == std::string::npos)
			{
				std::cerr << "(!)" << "State was not found in state vector." << std::endl;
				return false;
			}
		}

		for (unsigned int j = 0; j < m_alphabet.size(); j++)
		{
			if (i.m_state.find(m_alphabet[j]) != std::string::npos)
			{
				std::cerr << "(!)" << "Alphabet symbol was found in a state." << std::endl;
				return false;
			}
		}
		for (unsigned int j = 0; j < m_alphabet.size(); j++)
		{
			if (i.m_newState.find(m_alphabet[j]) != std::string::npos)
			{
				std::cerr << "(!)" << "Alphabet symbol was found in a state." << std::endl;
				return false;
			}
		}
		for (unsigned int j = 0; j < m_state.size(); j++)
		{
			if (m_state[j].find(i.m_symbol) != std::string::npos)
			{
				std::cerr << "(!)" << "Alphabet symbol was found in a state." << std::endl;
				return false;
			}
		}

		if (i.m_state == m_initialState)
		{
			initialStateCheck = true;
		}
		if (std::find(m_finalState.begin(), m_finalState.end(), i.m_newState) != m_finalState.end())
		{
			finalStateCheck = true;
		}
	}
	if (!initialStateCheck || !finalStateCheck)
	{
		std::cerr << "(!)" << "Foreign state in state vector." << std::endl;
		return false;
	}

	return true;
}

bool FiniteAutomaton::CheckAutomatonInputFile()
{
	std::ifstream input("inputFiniteAutomaton.txt");
	if (!input.is_open())
	{
		std::cerr << "(!)" << "Unable to open file." << std::endl;
		return false;
	}

	std::string testString;
	std::vector<std::string> inputVector;

	while (!input.eof())
	{
		input >> testString;
		inputVector.push_back(testString);
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_Q_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for state vector." << std::endl;
		return false;
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_SIGMA_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for Alphabet vector." << std::endl;
		return false;
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_START_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for Start character." << std::endl;
		return false;
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_FINAL_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for final states." << std::endl;
		return false;
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_T_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for T vector." << std::endl;
		return false;
	}

	return true;
}

bool FiniteAutomaton::CheckWord(std::string word)
{
	if (word.empty())
	{
		if (std::find(m_finalState.begin(), m_finalState.end(), m_initialState) == m_finalState.end() && !lambda)
		{
			std::cerr << "(!)" << "0-sized word with no initial state that is also a final state." << std::endl;
			return false;
		}
		else
		{
			return true;
		}
	}

	for (unsigned int i = 0; i < word.size(); i++)
	{
		if (std::find(m_alphabet.begin(), m_alphabet.end(), word[i]) == m_alphabet.end() && word[i] != lambda)
		{
			std::cerr << "(!)" << "Foreign symbols in word." << std::endl;
			return false;
		}
	}

	std::vector<std::string> initialState;
	initialState.push_back(m_initialState);
	std::vector<std::string> finalState;

	for (unsigned int i = 0; i < word.size(); i++)
	{
		finalState = {};
		for (unsigned int j = 0; j < initialState.size(); j++)
		{
			for (unsigned int k = 0; k < m_transition.size(); k++)
			{
				if (m_transition[k].m_state == initialState[j] && m_transition[k].m_symbol == word[i])
				{
					finalState.push_back(m_transition[k].m_newState);
				}
			}
		}
		initialState.clear();
		initialState = finalState;

	}

	bool finalCheck = false;
	for (unsigned int i = 0; i < m_finalState.size(); i++)
	{
		if (std::find(initialState.begin(), initialState.end(), m_finalState[i]) != initialState.end())
		{
			finalCheck = true;
		}
	}

	return finalCheck;
}

bool FiniteAutomaton::IsDeterministic()
{
	for (unsigned int i = 0; i < m_transition.size() - 1; i++)
	{
		for (unsigned int j = i + 1; j < m_transition.size(); j++)
		{
			if (m_transition[i].m_state == m_transition[j].m_state)
			{
				if (m_transition[i].m_symbol == m_transition[j].m_symbol)
				{
					if (m_transition[i].m_newState != m_transition[j].m_newState)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}

std::ostream& operator<<(std::ostream& flux, const Tranzition& tranzition)
{
	flux << tranzition.m_state << " -" << tranzition.m_symbol << "-> ";
	flux << tranzition.m_newState << std::endl;
	return flux;
}

std::ostream& operator<<(std::ostream& flux, const FiniteAutomaton& finiteAutomaton)
{
	flux << "Fie Automatul : M({";
	for (unsigned int i = 0; i < finiteAutomaton.m_state.size(); i++)
	{
		flux << finiteAutomaton.m_state[i];
		if (i < finiteAutomaton.m_state.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "},{";
	for (unsigned int i = 0; i < finiteAutomaton.m_alphabet.size(); i++)
	{
		flux << finiteAutomaton.m_alphabet[i];
		if (i < finiteAutomaton.m_alphabet.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "},{";
	flux << finiteAutomaton.m_initialState;
	flux << "},{";
	for (unsigned int i = 0; i < finiteAutomaton.m_finalState.size(); i++)
	{
		flux << finiteAutomaton.m_finalState[i];
		if (i < finiteAutomaton.m_finalState.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "}, T}";
	flux << ", T continand urmatoarele tranzitii:" << std::endl;
	unsigned int counter = 0;
	for (const auto& i : finiteAutomaton.m_transition)
	{
		flux << "(" << counter << ") " << i;
		counter++;
	}
	flux << std::endl;
	return flux;
}
