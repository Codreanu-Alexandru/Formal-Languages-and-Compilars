#pragma once
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <regex>
#include <set>

class Grammar
{
public:
	/*Constructors*/
	Grammar();

	/*Public operators*/
	friend std::ostream& operator<<(std::ostream& flux, const Grammar& grammar);
	friend std::istream& operator>>(std::istream& flux, Grammar& grammar);

public:
	/*Public functions*/
	std::string GenerateWords(unsigned int n);
	void PrintWords();
	void ClearWords();

	/*Public check functions*/
	bool CheckGrammerValidity();
	bool CheckGrammerInputFile();


	/*Getters*/
	char GetStartSymbol();
	std::vector<char> GetNonterminal();
	std::vector<char> GetTerminal();
	std::vector<std::pair<char, std::string>> GetProduction();

public:
	/*Destructor*/
	~Grammar();

private:
	/*Private memebrs*/
	char m_startSymbol;
	std::set<std::string> m_words;
	std::vector<char> m_nonterminal;
	std::vector<char> m_terminal;
	std::vector<std::pair<char, std::string>> m_productions;
	char lambda;

private:
	/*Private functions*/
	bool CouldBeAFinishedWord(const std::string& string);
	std::vector<int> GetPossibleRulePositions(const std::string&, unsigned int ruleNumber);
	void ApplyRule(std::string& string, unsigned int appliedPosition, unsigned int ruleNumber);
};

