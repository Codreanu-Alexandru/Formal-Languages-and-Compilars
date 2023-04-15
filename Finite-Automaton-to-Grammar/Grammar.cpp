#include "Grammar.h"

Grammar::Grammar()
{
	if (CheckGrammerInputFile())
	{
		std::ifstream input("inputGrammar.txt");
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
				if (state == "_VN_")
				{
					m_nonterminal.push_back(in[0]);
				}
				else if (state == "_VT_")
				{
					m_terminal.push_back(in[0]);
				}
				else if (state == "_START_")
				{
					m_startSymbol = in[0];
				}
				else if (state == "_P_")
				{
					std::pair<char, std::string> aux;
					aux.first = in[0];
					input >> in;
					aux.second = in;
					m_productions.push_back(aux);
				}
				else if (state == "_LAMBDA_")
				{
					lambda = in[0];
				}
			}
		}
		if (!CheckGrammerValidity())
		{
			std::cerr << "(!)" << "CAUTION! GRAMMAR NOT VALID!" << std::endl;
		}
		input.close();
	}
	else
	{
		std::cerr << "(!)" << "GRAMMAR INPUT NOT VALID!" << std::endl;
	}
}

bool Grammar::CheckGrammerValidity()
{
	/*Zeroth Check*/
	bool startingCheck = false;
	bool lambdaCheck = false;
	for (auto& i : m_productions)
	{
		if (i.second.find(lambda) != std::string::npos && i.first == m_startSymbol)
		{
			startingCheck = true;
		}
		if (i.second.find(m_startSymbol) != std::string::npos)
		{
			lambdaCheck = true;
		}
	}
	if (lambdaCheck && startingCheck)
	{
		std::cerr << "(!)" << "Lambda check fail." << std::endl;
		return false;
	}

	/*First Check*/
	for (unsigned int i = 0; i < m_nonterminal.size(); i++)
	{
		for (unsigned int j = 0; j < m_terminal.size(); j++)
		{
			if (std::find(m_nonterminal.begin(), m_nonterminal.end(), m_terminal[j]) != m_nonterminal.end())
			{
				std::cerr << "(!)" << "Vn has characters from Vt." << std::endl;
				return false;
			}

			if (std::find(m_terminal.begin(), m_terminal.end(), m_nonterminal[i]) != m_terminal.end())
			{
				std::cerr << "(!)" << "Vt has characters from Vn." << std::endl;
				return false;
			}
		}
	}
	
	/*Second Check*/
	if (std::find(m_nonterminal.begin(), m_nonterminal.end(), m_startSymbol) == m_nonterminal.end())
	{
		std::cerr << "(!)" << "Vn does not contain the Starter character." << std::endl;
		return false;
	}
	
	/*Third Check*/
	for (const std::pair<char, std::string>& i : m_productions)
	{
		if (std::find(m_nonterminal.begin(), m_nonterminal.end(), i.first) == m_nonterminal.end())
		{
			std::cerr << "(!)" << "At least a rule does not contain at least one non-terminal character." << std::endl;
			return false;
		}
	}
	
	/*Fourth Check*/
	unsigned int numberOfStarterProductions = 0;
	for (unsigned int i = 0; i < m_productions.size(); i++)
	{
		if (m_productions[i].first == m_startSymbol)
		{
			numberOfStarterProductions++;
		}
	}
	if (!numberOfStarterProductions)
	{
		std::cerr << "(!)" << "No production has been found with Starter character." << std::endl;
		return false;
	}

	/*Fifth Check*/
	for (int i = 0; i < m_productions.size(); i++)
	{
		unsigned int checkCounter = 0;
		for (unsigned int j = 0; j < m_nonterminal.size(); j++)
		{
			if (m_nonterminal[j] == m_productions[i].first)
			{
				checkCounter++;
			}
		}
		if (checkCounter == 0)
		{
			std::cerr << "(!)" << "Foreign character found in neither Vn or Vt." << std::endl;
			return false;
		}
		checkCounter = 0;
		for (int j = 0; j < m_nonterminal.size(); j++)
		{
			if (m_productions[i].second.find(m_nonterminal[j]) != std::string::npos)
			{
				checkCounter++;
			}
		}
		for (unsigned int j = 0; j < m_terminal.size(); j++)
		{
			if (m_productions[i].second.find(m_terminal[j]) != std::string::npos)
			{
				checkCounter++;
			}
		}
		if (checkCounter < m_productions[i].second.size() -1 && m_productions[i].second.find(lambda) == std::string::npos)
		{
			std::cerr << "(!)" << "Foreign character found in neither Vn or Vt." << std::endl;
			return false;
		}
	}

	/*All Checks Passed*/
	return true;
}

std::string Grammar::GenerateWords(unsigned int n)
{
	ClearWords();
	if (CheckGrammerValidity())
	{
		std::string lastWord;
		std::random_device generator;
		std::uniform_int_distribution<int> ruleDistribution(0, static_cast<int>(m_productions.size() - 1));

		while (m_words.size() < n)
		{
			std::string startString;
			startString.append(1, m_startSymbol);

			std::cout << startString << " -> ";
			while (!CouldBeAFinishedWord(startString))
			{
				int rule = ruleDistribution(generator);
				std::vector<int> possibleRulePositions = GetPossibleRulePositions(startString, rule);
				if (possibleRulePositions.empty())
				{
					while (possibleRulePositions.empty())
					{
						rule = ruleDistribution(generator);
						possibleRulePositions = GetPossibleRulePositions(startString, rule);
					}
				}
				std::uniform_int_distribution<int> positionDistribution(0, static_cast<int>(possibleRulePositions.size() - 1));
				int position = positionDistribution(generator);

				ApplyRule(startString, possibleRulePositions[position], rule);
				std::cout << startString;
				if (!CouldBeAFinishedWord(startString))
				{
					std::cout << " -> ";
				}
			}

			if (m_words.count(startString) <= 0)
			{
				m_words.insert(startString);
				std::cout << std::endl;
			}
			lastWord = startString;
		}
		return lastWord;
	}
	else
	{
		std::cerr << "(!)" << "Grammar is not valid." << std::endl;
		return {};
	}
}

void Grammar::PrintWords()
{
	if (m_words.empty())
	{
		std::cerr << "No words to be found." << std::endl;
		return;
	}
	std::cout << "The words are :" << std::endl;
	for (const auto& i : m_words)
	{
		std::cout << i << std::endl;
	}
}

void Grammar::ClearWords()
{
	m_words.clear();
}

bool Grammar::CheckGrammerInputFile()
{
	std::ifstream input("inputGrammar.txt");
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

	if (std::find(inputVector.begin(), inputVector.end(), "_VN_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for Vn vector." << std::endl;
		return false;
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_VT_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for Vt vector." << std::endl;
		return false;
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_START_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for Start character." << std::endl;
		return false;
	}

	if (std::find(inputVector.begin(), inputVector.end(), "_P_") == inputVector.end())
	{
		std::cerr << "(!)" << "No propper declaration for P vector." << std::endl;
		return false;
	}

	return true;
}

char Grammar::GetStartSymbol()
{
	return m_startSymbol;
}

std::vector<char> Grammar::GetNonterminal()
{
	return m_nonterminal;
}

std::vector<char> Grammar::GetTerminal()
{
	return m_terminal;
}

std::vector<std::pair<char, std::string>> Grammar::GetProduction()
{
	return m_productions;
}

Grammar::~Grammar()
{
	m_nonterminal.clear();
	m_terminal.clear();
	m_productions.clear();
	m_words.clear();
}

void Grammar::ApplyRule(std::string& string, unsigned int appliedPosition, unsigned int ruleNumber)
{
	
	if (ruleNumber > m_productions.size())
	{
		std::cerr << "(!)" << "Rule number out of bounds." << std::endl;
		return;
	}
	if (appliedPosition > string.size())
	{
		std::cerr << "(!)" << "Applied position out of bounds." << std::endl;
		return;
	}
	if (string.empty())
	{
		std::cerr << "(!)" << "Empty string." << std::endl;
		return;
	}
	if (string[appliedPosition] == m_productions[ruleNumber].first)
	{
		std::string firstHalf;
		std::string secondHalf;
		for (unsigned int i = 0; i < string.size(); i++)
		{
			if (i < appliedPosition)
			{
				firstHalf += string[i];
			}
			else if (i > appliedPosition )
			{
				secondHalf += string[i];
			}
		}
		if (m_productions[ruleNumber].second.find(lambda) == std::string::npos)
		{
			string = firstHalf + m_productions[ruleNumber].second + secondHalf;
		}
		else
		{
			string = firstHalf + secondHalf;
		}
	}
	else
	{
		std::cerr << "(!)" << "Rule can't be applied." << std::endl;
		return;
	}
	
}

bool Grammar::CouldBeAFinishedWord(const std::string& string)
{
	for (unsigned int i = 0; i < string.size(); i++)
	{
		bool inclustionCheck = false;
		for (int j = 0; j < m_terminal.size(); j++)
		{
			if (string[i] == m_terminal[j])
			{
				inclustionCheck = true;
			}
		}
		if (!inclustionCheck)
		{
			return false;
		}
	}

	return true;
}

std::vector<int> Grammar::GetPossibleRulePositions(const std::string& string, unsigned int ruleNumber)
{
	std::vector<int> possibleRulePositions;
	if (ruleNumber > m_productions.size())
	{
		std::cerr << "(!)" << "Rule number out of bounds." << std::endl;
		return std::vector<int>();
	}
	if (string.empty())
	{
		std::cerr << "(!)" << "Empty string." << std::endl;
		return std::vector<int>();
	}
	for (int i = 0; i < string.size(); i++)
	{
		if (string[i] == m_productions[ruleNumber].first)
		{
			possibleRulePositions.push_back(i);
		}
	}
	return possibleRulePositions;
}

std::ostream& operator<<(std::ostream& flux, const Grammar& grammar)
{
	flux << "Fie gramatica : G({";
	for (unsigned int i = 0; i < grammar.m_nonterminal.size(); i++)
	{
		flux << grammar.m_nonterminal[i];
		if (i < grammar.m_nonterminal.size()-1)
		{
			flux << ", ";
		}
	}
	flux << "},{";
	for (unsigned int i = 0; i < grammar.m_terminal.size(); i++)
	{
		flux << grammar.m_terminal[i];
		if (i < grammar.m_terminal.size()-1)
		{
			flux << ", ";
		}
	}
	flux << "}, ";
	flux << grammar.m_startSymbol;
	flux << ", P}";
	flux << ", P continand urmatoarele productii:" << std::endl;
	for (unsigned int i = 0; i < grammar.m_productions.size(); i++)
	{
		flux << "(" << i << ") ";
		flux << grammar.m_productions[i].first << " -> " << grammar.m_productions[i].second;
		if (i < grammar.m_productions.size()-1)
		{
			flux << std::endl;
		}
	}
	flux << std::endl;
	return flux;
}

std::istream& operator>>(std::istream& flux, Grammar& grammar)
{
	std::cout << " Starting symbol : ";
	flux >> grammar.m_startSymbol;
	std::cout << "Non-terminal characters (Until \'" << grammar.lambda << "\' is inserted): " << std::endl;
	char auxChar;
	flux >> auxChar;
	while (auxChar != grammar.lambda)
	{
		grammar.m_nonterminal.push_back(auxChar);
		flux >> auxChar;
	}
	
	std::string auxString;
	std::cout << "Terminal characters (Until \'" << grammar.lambda << "\' is inserted twice without enter): " << std::endl;
	flux >> auxString;
	std::string aux(2, grammar.lambda);
	while (auxString != aux)
	{
		grammar.m_terminal.push_back(auxString[0]);
		flux >> auxString;
	}

	std::cout << "Productions (Must be a nonterminal symbol followed by string until the nonterminal symbol is " << "\'" << grammar.lambda << "\'):" << std::endl;
	flux >> auxChar;
	flux >> auxString;
	while (auxChar != grammar.lambda)
	{
		grammar.m_productions.push_back({ auxChar,auxString });
		flux >> auxChar;
		flux >> auxString;
	}
	return flux;
}
