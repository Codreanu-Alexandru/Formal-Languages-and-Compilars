#pragma once
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <regex>
#include <queue>
#include <set>

class Grammar
{
public:
	/*Constructors*/
	Grammar(bool readFromFile = true);

public:
	/*Public operators*/
	friend std::ostream& operator<<(std::ostream& flux, const Grammar& grammar);
	friend std::istream& operator>>(std::istream& flux, Grammar& grammar);

public:

	/*Public functions*/
	std::string GenerateWords(unsigned int n);
	Grammar ConvertToGreibachNormalForm();
	Grammar ConvertToChomskyNormalForm();
	Grammar SimplefyGrammar();
	void PrintWords();
	void ClearWords();

	/*Public check functions*/
	bool CheckContextFreeGrammar();
	bool CheckGrammerInputFile();
	bool CheckGrammerValidity();

	/*Getters*/
	std::vector<std::pair<char, std::string>> GetProductions();
	std::vector<char> GetNonterminals();
	std::vector<char> GetTerminals();
	char GetStartSymbol();

public:
	/*Destructor*/
	~Grammar();


private:
	/*Private memebrs*/
	std::vector<std::pair<char, std::string>> m_productions;
	std::vector<char> m_nonTerminals;
	std::vector<char> m_terminals;
	char m_startSymbol; 
	bool emptyGrammar;
	char lambda;

private:
	/*Private members - words*/
	std::set<std::string> m_words;

private:
	/*Private functions*/
	void ApplyRule(std::string& string, unsigned int appliedPosition, unsigned int ruleNumber);
	std::vector<int> GetPossibleRulePositions(const std::string&, unsigned int ruleNumber);
	bool CouldBeAFinishedWord(const std::string& string);

	std::vector<std::pair<char, std::string>> FirstLemaProductionChanges(std::pair<char, std::string> production, unsigned int index);
	std::vector<std::pair<char, std::string>> ModifiedFirstLemaProductionChanges(std::pair<char, std::string> production, unsigned int index);
	std::vector<std::pair<char, std::string>> SecondLemaProductionChanges(std::pair<char, std::string> production);
	std::vector<std::pair<char, std::string>> FindAllProductionsThatContain(char nonTerminal);
	std::vector<std::pair<char, std::string>> FindAllProductionsThatMatch(char nonTerminal);
	std::vector<std::pair<char, std::string>> ModifiedFindAllProductionsThatMatch(char nonTerminal);

	std::set<char> SimbolsThatDerive(std::vector<char> deriv);
	void RemoveUnitProductions();
	void RenameNonTerminals();
	void ReductionCFG();

	/*Private Check Functions*/
	bool CheckIfProductionIsValid(const std::pair<char, std::string>& production);
};

