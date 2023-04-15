#include <cstdint>
#include <iostream>
#include "PushDownAutomaton.h";
#include "Grammar.h"

int main()
{
	int option{ 0 };
	Grammar grammar;
	PushDownAutomaton pushDownAutomaton;
	pushDownAutomaton = pushDownAutomaton.ConvertFromGreibach(grammar.ConvertToGreibachNormalForm());

	while (true)
	{
		std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
		std::cout << "1 - Show grammar." << std::endl;
		std::cout << "2 - Generate a word from grammar." << std::endl;
		std::cout << "3 - Show simplified grammar." << std::endl;
		std::cout << "4 - Show Greibach Normal Form of grammar." << std::endl;
		std::cout << "5 - Check if generated word is accepted by Push Down Automaton." << std::endl;
		std::cout << "6 - Check if input word is accepted by Push Down Automaton." << std::endl;
		std::cout << "7 - Print Push Down Automaton." << std::endl;
		std::cout << "9 - EXIT" << std::endl;
		std::cout << "0 - Clear console." << std::endl;
		std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
		std::cout << "You chose : ";
		std::cin >> option;

		switch (option)
		{
		case 0:
		{
			system("CLS");
			break;
		}
		case 1:
		{
			std::cout << "Current grammar : " << std::endl;
			std::cout << grammar << std::endl;
			break;
		}
		case 2:
		{
			std::string auxWord = grammar.GenerateWords(1);
			std::cout << "The word generated : " << auxWord << std::endl;
			break;
		}
		case 3:
		{
			grammar = grammar.SimplefyGrammar();
			std::cout << "Simplified grammar : " << std::endl;
			std::cout << grammar << std::endl;
			break;
		}
		case 4:
		{
			grammar = grammar.ConvertToGreibachNormalForm();
			std::cout << "Greibach Normal Form grammar : " << std::endl;
			std::cout << grammar << std::endl;
			break;
		}
		case 5:
		{
			std::string auxWord = grammar.GenerateWords(1);
			std::cout << "The word generated : " << auxWord << std::endl;
			std::cout << "Is " << (pushDownAutomaton.CheckWord(auxWord) ? "Valid" : "not Valid") << std::endl;
			break;
		}
		case 6:
		{
			std::string auxWord{ "" };
			std::cout << "The word : ";
			std::cin >> auxWord;
			std::cout << "Is " << (pushDownAutomaton.CheckWord(auxWord) ? "Valid" : "not Valid") << std::endl;
			break;
		}
		case 7:
		{
			std::cout << "Push Down Automaton : " << std::endl;
			std::cout << pushDownAutomaton << std::endl;
			break;
		}
		case 9:
		{
			return 0;
		}
		default:
		{
			std::cout << "How did we get here ?" << std::endl;
			break;
		}
		}
	}
}