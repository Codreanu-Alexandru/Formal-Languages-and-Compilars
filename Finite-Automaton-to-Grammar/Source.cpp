#include "Grammar.h"
#include "FiniteAutomaton.h"

FiniteAutomaton ConvertGrammarToFiniteAutomaton(Grammar& grammar)
{
	std::vector<std::string> a_state;
	std::vector<char> a_alphabet;
	std::string a_initialState;
	std::vector<std::string> a_finalState;
	std::vector<Tranzition> a_transition;

	char g_startSymbol = grammar.GetStartSymbol();
	std::vector<char> g_nonterminal = grammar.GetNonterminal();
	std::vector<char> g_terminal = grammar.GetTerminal();
	std::vector<std::pair<char, std::string>> g_productions = grammar.GetProduction();

	a_initialState = g_startSymbol;
	for (unsigned int i = 0; i < g_nonterminal.size(); i++)
	{
		std::string aux(1, g_nonterminal[i]);
		a_state.push_back(aux);
	}

	std::string aux(1, 'F');
	a_finalState.push_back(aux);

	a_state.push_back(a_finalState[0]);

	for (unsigned int i = 0; i < g_terminal.size(); i++)
	{
		a_alphabet.push_back(g_terminal[i]);
	}

	for (const auto& i : g_productions)
	{
		Tranzition auxTranzition;
		auxTranzition.m_state = i.first;
		auxTranzition.m_symbol = i.second[0];
		if (i.second.size() >= 2)
		{
			auxTranzition.m_newState = i.second[1];
		}
		else
		{
			auxTranzition.m_newState = 'F';
		}
		a_transition.push_back(auxTranzition);
	}

	FiniteAutomaton finiteAutomaton(a_state, a_alphabet, a_initialState, a_finalState, a_transition);

	return finiteAutomaton;
}

int main()
{
	unsigned int op;
	Grammar g;
	FiniteAutomaton f(false);
	g.CheckGrammerValidity() ? op = 1 : op = 0;
	while (op)
	{
		std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
		std::cout << "1 - Print grammar." << std::endl;
		std::cout << "2 - Generate n words." << std::endl;
		std::cout << "3 - Get automaton from grammar." << std::endl;
		std::cout << "4 - Verify a word with automaton." << std::endl;
		std::cout << "5 - Generate a words and check if its accepted by the automaton." << std::endl;
		std::cout << "0 - Stop." << std::endl;
		std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
		std::cout << "Enter : ";
		std::cin >> op;

		switch (op)
		{
		case 0 :
		{
			break;
		}
		case 1:
		{
			std::cout << g;
			break;
		} 
		case 2:
		{
			unsigned int n;
			std::cout << "How many words ? ";
			std::cin >> n;
			g.GenerateWords(n);
			break;
		}
		case 3:
		{
			f = ConvertGrammarToFiniteAutomaton(g);
			std::cout << f;
			break;
		}
		case 4:
		{
			std::string aux;
			std::cout << "Enter word : ";
			std::cin >> aux;
			std::cout << (f.CheckWord(aux) ? "Its a valid word." : "It's not a valid word.") << std::endl;
			break;
		}
		case 5:
		{
			std::string aux = g.GenerateWords(1);
			std::cout << "The word generated : " << aux << std::endl;
			std::cout << (f.CheckWord(aux) ? "Its a valid word." : "It's not a valid word.") << std::endl;
			break;
		}
		default:
			break;
		}
	}
	return 0;
}