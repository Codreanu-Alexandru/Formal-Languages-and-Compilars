#include "Grammar.h"

Grammar::Grammar(bool readFromFile)
{
	if (readFromFile)
	{
		if (CheckGrammerInputFile())
		{
			std::ifstream input("inputG.txt");
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
						m_nonTerminals.push_back(in[0]);
					}
					else if (state == "_VT_")
					{
						m_terminals.push_back(in[0]);
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
	else
	{
		emptyGrammar = true;
	}
}

bool Grammar::CheckContextFreeGrammar()
{
	for (auto& i : m_productions)
	{
		bool hasTerminals = false;
		bool hasNonTerminals = false;
		for (auto& j : i.second)
		{
			hasTerminals = (std::find(m_terminals.begin(), m_terminals.end(), j) != m_terminals.end());
			hasNonTerminals = (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), j) != m_nonTerminals.end());

			if (!(hasTerminals || hasNonTerminals) && j != lambda)
			{
				return false;
			}
		}
		
		if (std::find(m_terminals.begin(), m_terminals.end(), i.first) != m_terminals.end())
		{
			return false;
		}

		if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), i.first) == m_nonTerminals.end())
		{
			return false;
		}
	}
	return true;
}

bool Grammar::CheckGrammerValidity()
{
	/*First Check*/
	for (unsigned int i = 0; i < m_nonTerminals.size(); i++)
	{
		for (unsigned int j = 0; j < m_terminals.size(); j++)
		{
			if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), m_terminals[j]) != m_nonTerminals.end())
			{
				std::cerr << "(!)" << "Vn has characters from Vt." << std::endl;
				return false;
			}

			if (std::find(m_terminals.begin(), m_terminals.end(), m_nonTerminals[i]) != m_terminals.end())
			{
				std::cerr << "(!)" << "Vt has characters from Vn." << std::endl;
				return false;
			}
		}
	}

	/*Second Check*/
	if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), m_startSymbol) == m_nonTerminals.end())
	{
		std::cerr << "(!)" << "Vn does not contain the Starter character." << std::endl;
		return false;
	}

	/*Third Check*/
	for (const std::pair<char, std::string>& i : m_productions)
	{
		if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), i.first) == m_nonTerminals.end())
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

	/*All Checks Passed*/
	return true;
}

Grammar Grammar::ConvertToGreibachNormalForm()
{
	/*1 : CNF*/
	Grammar newGrammar = ConvertToChomskyNormalForm();

	/*2 : Rename non terminals */
	newGrammar.RenameNonTerminals();

	/*3 : Adjust to Ai -> Aj | i < j */
	/*1st & 2nd Lema*/
	bool check = false;
	while (!check)
	{
		check = true;
		for (unsigned int i = 0; i < newGrammar.m_productions.size(); i++)
		{
			if (std::find(newGrammar.m_nonTerminals.begin(), newGrammar.m_nonTerminals.end(), newGrammar.m_productions[i].second[0])
				!= newGrammar.m_nonTerminals.end())
			{
				if (newGrammar.m_productions[i].first > newGrammar.m_productions[i].second[0])
				{
					check = false;
					auto aux = FirstLemaProductionChanges(newGrammar.m_productions[i], 0);
					if (!aux.empty())
					{
						newGrammar.m_productions[i] = aux[0];
					}
					for (unsigned int j = 1; j < aux.size(); j++)
					{
						newGrammar.m_productions.push_back(aux[j]);
					}
				}
				else if (newGrammar.m_productions[i].first == newGrammar.m_productions[i].second[0])
				{
					check = false;
					auto aux = SecondLemaProductionChanges(newGrammar.m_productions[i]);
					if (!aux.empty())
					{
						newGrammar.m_productions[i] = aux[0];
					}
					for (unsigned int j = 1; j < aux.size(); j++)
					{
						newGrammar.m_productions.push_back(aux[j]);
					}
				}
			}
		}
	}
	/*4 : Adjust for A->aB*/
	bool checkFirstNonTerminal = false;
	while (!checkFirstNonTerminal)
	{
		checkFirstNonTerminal = true;
		for (unsigned int i = 0; i < newGrammar.m_productions.size(); i++)
		{
			if (std::find(newGrammar.m_terminals.begin(), newGrammar.m_terminals.end(), newGrammar.m_productions[i].second[0])
				== newGrammar.m_terminals.end())
			{
				checkFirstNonTerminal = false;
				auto aux = newGrammar.ModifiedFirstLemaProductionChanges(newGrammar.m_productions[i], 0);
				if (!aux.empty())
				{
					newGrammar.m_productions[i] = aux[0];
				}
				for (unsigned int j = 1; j < aux.size(); j++)
				{
					newGrammar.m_productions.push_back(aux[j]);
				}
			}
		}
	}
	return newGrammar;
}

Grammar Grammar::SimplefyGrammar()
{
	Grammar newGrammar = this;
	newGrammar.ReductionCFG();
	newGrammar.RemoveUnitProductions();
	return newGrammar;
	
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
	std::ifstream input("inputG.txt");
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

std::vector<char> Grammar::GetNonterminals()
{
	return m_nonTerminals;
}

std::vector<char> Grammar::GetTerminals()
{
	return m_terminals;
}

std::vector<std::pair<char, std::string>> Grammar::GetProductions()
{
	return m_productions;
}

Grammar::~Grammar()
{
	m_nonTerminals.clear();
	m_terminals.clear();
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
			else if (i > appliedPosition)
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

std::set<char> Grammar::SimbolsThatDerive(std::vector<char> deriv)
{
	std::set<char>newElements;

	for (const auto& der : deriv)
	{
		for (const auto& i : m_productions)
		{
			if (i.second.find(der) != std::string::npos)
			{
				newElements.insert(i.first);
			}
		}
	}
	return newElements;
}

std::vector<std::pair<char, std::string>> Grammar::FindAllProductionsThatMatch(char nonTerminal)
{
	std::vector<std::pair<char, std::string>> productions;
	for (const auto& i : m_productions)
	{
		if (i.first == nonTerminal)
		{
			productions.push_back(i);
		}
	}

	return productions;
}

std::vector<std::pair<char, std::string>> Grammar::ModifiedFindAllProductionsThatMatch(char nonTerminal)
{
	std::vector<std::pair<char, std::string>> productions;
	for (const auto& i : m_productions)
	{
		if (i.first == nonTerminal &&
			i.second.size() == 1 &&
			std::find(m_terminals.begin(), m_terminals.end(), i.second[0])
			!= m_terminals.end())
		{
			productions.push_back(i);
		}
	}

	return productions;
}

std::vector<std::pair<char, std::string>> Grammar::FindAllProductionsThatContain(char nonTerminal)
{
	std::vector<std::pair<char, std::string>> result{};

	for (auto& i : m_productions)
	{
		if (i.first == nonTerminal)
		{
			result.push_back(i);
		}
		else
		{
			for (auto& j : i.second)
			{
				if (j == nonTerminal)
				{
					result.push_back(i);
					break;
				}
			}
		}
	}
	return result;
}

bool Grammar::CheckIfProductionIsValid(const std::pair<char, std::string>&production)
{
	if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), production.first) == m_nonTerminals.end())
	{
		return false;
	}

	//std::stringstream ss;
	for (const auto& element : production.second)
	{
		//ss << element;
		if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), element) == m_nonTerminals.end())
		{

			if (std::find(m_terminals.begin(), m_terminals.end(), element) == m_terminals.end())
			{
				if (element != lambda)
				{
					return false;
				}
			}
		}
		
		//ss.str("");
	}
	return true;
}

Grammar Grammar::ConvertToChomskyNormalForm()
{
	/*TO DO*/
	/*Step 0 : Check if grammar is context free*/
	if (!CheckContextFreeGrammar())
	{
		std::cerr << "(!)" << "Grammar is not Context-Free" << std::endl;
		return Grammar(false);
	}

	/*Initiate new grammar*/
	Grammar newGrammar = this;

	/*Step 1 : Eliminate start symbol from right hand side of the productions*/
	bool startingSymbolViolation = false;
	for (auto& i : m_productions)
	{
		if (i.second.find(m_startSymbol) != std::string::npos)
		{
			startingSymbolViolation = true;
		}
	}
	if (startingSymbolViolation)
	{
		std::pair<char, std::string> newStart;
		newStart.first = '@';
		newStart.second = std::string(1, m_startSymbol);
		newGrammar.m_productions.push_back(newStart);
		newGrammar.m_startSymbol = '@';
	}

	/*Step 2 : Eliminate uselles, null or unit productions*/
	newGrammar = newGrammar.SimplefyGrammar();

	/*Step 3 : Enforce rule A -> a|BC...*/
	std::vector<std::pair<char, std::string>> thirdStepProductions;
	unsigned int index = 0;
	for (auto& i : newGrammar.m_productions)
	{
		bool alreadyAdded = false;
		if (i.second.size() == 1)
		{
			thirdStepProductions.push_back(i);
			alreadyAdded = true;
		}
		else
		{
			int index = 0;
			bool skip = false;
			for (auto& j : i.second)
			{
				if (!skip)
				{
					if (std::find(newGrammar.m_terminals.begin(), newGrammar.m_terminals.end(), j)
						!= newGrammar.m_terminals.end())
					{
						index++;
						std::pair<char, std::string> newProductionNeeded{ (char)(j + index - 32),std::string(1,j) };
						i.second.insert(i.second.begin() + index, (char)(j - 32));
						skip = true;
						thirdStepProductions.push_back(newProductionNeeded);
						newGrammar.m_nonTerminals.push_back((char)(j - 32));
					}

				}
				else
				{
					skip = false;
				}
				index++;
			}

		}
		if (!alreadyAdded)
		{
			thirdStepProductions.push_back(i);
		}
	}

	/*Step 4 : Enforce rule A -> BC*/

	std::vector<std::pair<char, std::string>> forthStepProductions;
	for (unsigned int i = 0; i < thirdStepProductions.size(); i++)
	{
		if (thirdStepProductions[i].second.size() > 2)
		{
			std::pair<char, std::string> newProductionNeeded{
				thirdStepProductions[i].first,
				std::string(1,(char)(thirdStepProductions[i].first + index))};

			std::pair<char, std::string> nextNewProductionNeeded{
				(char)(thirdStepProductions[i].first + index),
				(thirdStepProductions[i].second.begin() + 3)._Unwrapped() };

			thirdStepProductions.push_back(newProductionNeeded);
			//thirdStepProductions.insert(thirdStepProductions.begin() + i, newProductionNeeded);
			thirdStepProductions.push_back(nextNewProductionNeeded);
			//thirdStepProductions.insert(thirdStepProductions.begin() + i + 1, nextNewProductionNeeded);
			newGrammar.m_nonTerminals.push_back((char)(thirdStepProductions[i].first + index));
		}
		else
		{
			forthStepProductions.push_back(thirdStepProductions[i]);
		}
		index++;
	}

	newGrammar.m_productions.clear();
	newGrammar.m_productions = forthStepProductions;

	return newGrammar;
}

void Grammar::ReductionCFG()
{	//std::stringstream ss;
	
	std::vector<char> stringTerminals;
	for (const auto& terminals : m_terminals)
	{
		//ss << terminals;
		stringTerminals.push_back(terminals);
		//ss.str("");
	}

	std::set<char> wPrevious;
	std::set<char> wCurrent = SimbolsThatDerive(stringTerminals);
	while (wPrevious != wCurrent)
	{
		wPrevious = wCurrent;
		auto newSet = SimbolsThatDerive(std::vector<char>(wCurrent.begin(), wCurrent.end()));
		for (const auto& el : newSet)
		{
			wCurrent.insert(el);
		}

	} 
	m_nonTerminals = std::vector<char>(wCurrent.begin(), wCurrent.end());

	for (const auto& production : m_productions)
	{
		if (!CheckIfProductionIsValid(production))
		{
			m_productions.erase(
				std::remove(m_productions.begin(), m_productions.end(),production),
				m_productions.end());
		}
	}

	std::vector<char> newTerminals;
	std::vector<char> newNonTerminals;
	std::queue<char> currentListToCheck;
	currentListToCheck.push(m_startSymbol);
	while(!currentListToCheck.empty())
	{	

		auto listOfProduction = FindAllProductionsThatMatch(currentListToCheck.front());
		
		//ss<<currentListToCheck.front();
			if (std::find(newNonTerminals.begin(), newNonTerminals.end(), currentListToCheck.front()) == newNonTerminals.end()) {
				newNonTerminals.push_back(currentListToCheck.front());
			}

		currentListToCheck.pop();
		//ss.str("");
		for (const auto& production : listOfProduction)
		{
			for(const auto& element:production.second)
			{
				
				//ss << element;
				if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), element) == m_nonTerminals.end())
				{
					if (std::find(newTerminals.begin(), newTerminals.end(), element) == newTerminals.end()) {
						newTerminals.push_back(element);
					}
				}
				else
				{
					currentListToCheck.push(element);
				}
				//ss.str("");
			}
		}
	}
	m_nonTerminals=newNonTerminals;
	m_terminals=newTerminals;

	for(int i=0;i<m_productions.size();i++)
	{
		if (!CheckIfProductionIsValid(m_productions[i]))
		{
			m_productions.erase(m_productions.begin()+i);
			i--;
		}
	}
}

void Grammar::RenameNonTerminals()
{
	std::vector<std::pair<char, char>> terminalPairs;
	std::vector<std::pair<char, std::string>> newProductions;
	std::vector<char> newNonterminals;
	char newStartSymbol = 'A';
	char newSymbol = 'A';

	for (unsigned int i = 0; i < m_nonTerminals.size(); i++)
	{
		terminalPairs.push_back({newSymbol,m_nonTerminals[i]});
		if (m_nonTerminals[i] == m_startSymbol)
		{
			newStartSymbol = newSymbol;
		}
		newNonterminals.push_back(newSymbol);
		newSymbol += 1;
	}

	for (auto& p : m_productions)
	{
		std::string newLHS{ "" };
		std::string newRHS{ "" };
		for (auto& it : terminalPairs)
		{
			if (p.first == it.second)
			{
				newLHS += it.first;
			}
		}
		for (auto& j : p.second)
		{
			if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), j) != m_nonTerminals.end())
			{
				for (auto& k : terminalPairs)
				{
					if (j == k.second)
					{
						newRHS += k.first;
					}
				}
			}
			else
			{
				newRHS += j;
			}
		}
		if (!newLHS.empty())
		{
			if (!newRHS.empty())
			{
				newProductions.push_back({ newLHS[0],newRHS});
			}
			else
			{
				newProductions.push_back({ newLHS[0],p.second});
			}
		}
		else
		{
			if (!newRHS.empty())
			{
				newProductions.push_back({ p.first,newRHS });
			}
			else
			{
				newProductions.push_back({ p.first,p.second });
			}
		}
	}

	m_nonTerminals = newNonterminals;
	m_productions = newProductions;
	m_startSymbol = newStartSymbol;
	return;
}

std::vector<std::pair<char, std::string>> Grammar::FirstLemaProductionChanges(std::pair<char, std::string> production, unsigned int index)
{
	std::vector<std::pair<char, std::string>> newProductions;
	std::vector<std::pair<char, std::string>> productionsToBeMatched = FindAllProductionsThatMatch(production.second[index]);

	for (unsigned int i = 0; i < productionsToBeMatched.size(); i++)
	{
		if (index >= 1)
		{
			newProductions.push_back({
				production.first,
				production.second.substr(0,index - 1) +
				productionsToBeMatched[i].second +
				production.second.substr(production.second.size() - index - 1) });
		}
		else
		{
			newProductions.push_back({
				production.first,
				productionsToBeMatched[i].second + production.second.substr(production.second.size() - index - 1) });
		}
	}

	return newProductions;
}

std::vector<std::pair<char, std::string>> Grammar::ModifiedFirstLemaProductionChanges(std::pair<char, std::string> production, unsigned int index)
{
	std::vector<std::pair<char, std::string>> newProductions;
	std::vector<std::pair<char, std::string>> productionsToBeMatched = ModifiedFindAllProductionsThatMatch(production.second[index]);

	for (unsigned int i = 0; i < productionsToBeMatched.size(); i++)
	{
		if (index >= 1)
		{
			newProductions.push_back({
				production.first,
				production.second.substr(0,index) +
				productionsToBeMatched[i].second +
				production.second.substr(production.second.size() - index) });
		}
		else
		{
			newProductions.push_back({
				production.first,
				productionsToBeMatched[i].second + production.second.substr(1,production.second.size() - 1) });
		}
	}

	return newProductions;
}

std::vector<std::pair<char, std::string>> Grammar::SecondLemaProductionChanges(std::pair<char, std::string> production)
{
	std::vector<std::pair<char, std::string>> newProductions{};

	if (production.first == production.second[0])
	{
		newProductions.push_back({ 'Z',production.second.substr(0) + "Z" });
		newProductions.push_back({ 'Z',production.second.substr(0) });
	}
	else if (production.second.size() == 1)
	{
		newProductions.push_back({ production.first,production.second + "Z" });
	}

	return newProductions;
}

void Grammar::RemoveUnitProductions()
{
	for (const auto& production : m_productions)
	{
		if (production.second.size() == 1)
		{
			if (std::find(m_nonTerminals.begin(), m_nonTerminals.end(), production.second[0]) != m_nonTerminals.end())
			{
				auto listOfProduction = FindAllProductionsThatMatch(production.second[0]);
				for (const auto& newProduction : listOfProduction)
				{
					m_productions.push_back(make_pair(production.first, newProduction.second));
				}
		
			m_productions.erase(
				std::remove(m_productions.begin(), m_productions.end(), production),
				m_productions.end());
			}	
			
		}
	}
}

bool Grammar::CouldBeAFinishedWord(const std::string& string)
{
	for (unsigned int i = 0; i < string.size(); i++)
	{
		bool inclustionCheck = false;
		for (int j = 0; j < m_terminals.size(); j++)
		{
			if (string[i] == m_terminals[j])
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
	for (unsigned int i = 0; i < string.size(); i++)
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
	for (unsigned int i = 0; i < grammar.m_nonTerminals.size(); i++)
	{
		flux << grammar.m_nonTerminals[i];
		if (i < grammar.m_nonTerminals.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "},{";
	for (unsigned int i = 0; i < grammar.m_terminals.size(); i++)
	{
		flux << grammar.m_terminals[i];
		if (i < grammar.m_terminals.size() - 1)
		{
			flux << ", ";
		}
	}
	flux << "}, ";
	flux << grammar.m_startSymbol;
	flux << ", P)";
	flux << ", P continand urmatoarele productii:" << std::endl;
	for (unsigned int i = 0; i < grammar.m_productions.size(); i++)
	{
		flux << "(" << i << ") ";
		flux << grammar.m_productions[i].first << " -> " << grammar.m_productions[i].second;
		if (i < grammar.m_productions.size() - 1)
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
		grammar.m_nonTerminals.push_back(auxChar);
		flux >> auxChar;
	}

	std::string auxString;
	std::cout << "Terminal characters (Until \'" << grammar.lambda << "\' is inserted twice without enter): " << std::endl;
	flux >> auxString;
	std::string aux(2, grammar.lambda);
	while (auxString != aux)
	{
		grammar.m_terminals.push_back(auxString[0]);
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
