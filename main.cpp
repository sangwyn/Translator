#include <iostream>
#include <fstream>
#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"

int main() {
	std::cout << '\n';
	// LEXICAL ANALYSIS
	std::ifstream fin("input.txt");
	std::string input = "";
	while (!fin.eof()) {
		std::string s;
		std::getline(fin, s);
		input += s;
		input.push_back('\n');
	}
	fin.close();

	LexicalAnalyzer lexical_analyzer;
	try {
		auto lexemes = lexical_analyzer.Analyze(input);
		std::ofstream fout("lexemes.txt");
		fout << lexemes[0].first << ' ' << lexemes[0].second.first << ' ' << lexemes[0].second.second;
		for (int i = 1; i < lexemes.size(); ++i) {
			fout << '\n' << lexemes[i].first << ' ' << lexemes[i].second.first << ' ' << lexemes[i].second.second;
		};
		//        for (auto i: lexemes) {
		//            fout << i.first << " " << i.second << '\n';
		//        }
		fout << "\n_ 0 0";
		fout.close();
		std::cout << "Lexemes from 'input.txt' saved to 'lexemes.txt'.\n\n";
	}
	catch (char c) {
		std::cout << "Invalid character in input: '" << c << "'.";
		return 1;
	}

	// SYNTAX ANALYSIS
	SyntaxAnalyzer syntax_analyzer;
	try {
		try {
			syntax_analyzer.Analyze();
			std::cout << "Syntax analysis was executed correctly.\n\n";
		}
		catch (const std::exception &error) {
			if (error.what() == "ok") {
				throw error;
			}
			if (error.what()[5] == '0')
				std::cout << "No 'string main()' function found.";
			else
				std::cout << error.what();
			return 1;
		}
	}
	catch (const std::exception&) {
		std::cout << "Syntax analysis was executed correctly.\n\n";
	}

	// SEMANTIC ANALYSIS
	// GENERATING INTERNAL REPRESENTATION
	// RUNNING PROGRAM

	return 0;
}
