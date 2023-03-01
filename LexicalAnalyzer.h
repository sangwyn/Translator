#ifndef TRANSLATOR__LEXICALANALYZER_H_
#define TRANSLATOR__LEXICALANALYZER_H_

#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include <cctype>
#include <fstream>

class LexicalAnalyzer {
private:
	bool IsOperation(char c);
	bool IsBigOperation(std::string c);
	bool IsPunctuation(char c);
	bool IsBracket(char c);
	bool IsSqBracket(char c);
	bool IsSpace(char c);
	bool IsCommentStart(char c);
	bool IsReserved(const std::vector<std::string> reserved, const std::string &word);

	void ReadReservedWords(std::vector<std::string> &list);

public:
	std::vector<std::pair<std::string, std::pair<int, int>>> Analyze(std::string input);
};

#endif
