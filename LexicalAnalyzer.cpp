#include "LexicalAnalyzer.h"

bool LexicalAnalyzer::IsOperation(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' || c == '&' || c == '>'
		|| c == '<' || c == '!' || c == '=';
}

bool LexicalAnalyzer::IsBigOperation(std::string c) {
	return c == "+=" || c == "-=" || c == "*=" || c == "/=" || c == "==" || c == "!=" || c == "&&" || c == "||" || c == "<=" || c == ">=" || c == "//";
}

bool LexicalAnalyzer::IsPunctuation(char c) {
	return c == ';' || c == '{' || c == '}' || c == ',' || c == '.';
}

bool LexicalAnalyzer::IsBracket(char c) {
	return c == '(' || c == ')';
}

bool LexicalAnalyzer::IsSqBracket(char c) {
	return c == '[' || c == ']';
}

bool LexicalAnalyzer::IsSpace(char c) {
	return c == ' ' || c == '\n' || c == '\t';
}

bool LexicalAnalyzer::IsCommentStart(char c) {
	return c == '#';
}

bool LexicalAnalyzer::IsReserved(const std::vector<std::string> reserved, const std::string &word) {
	for (auto &s : reserved) {
		if (s == word)
			return true;
	}
	return false;
}

void LexicalAnalyzer::ReadReservedWords(std::vector<std::string> &list) {
	std::fstream input;
	input.open("ReservedInput.txt");
	if (!input.is_open()) {
		throw "Cannot open the file";
	}
	while (!input.eof()) {
		std::string word;
		std::getline(input, word);
		list.push_back(word);
	}
	input.close();
}

std::vector<std::pair<std::string, std::pair<int, int>>> LexicalAnalyzer::Analyze(std::string input) {
	input += '\n';
	std::vector<std::pair<std::string, std::pair<int, int>>> res;
	std::vector<std::string> reserved_words;
	ReadReservedWords(reserved_words);
	int state = 0;
	std::string cur_word = "";
	int line = 1;
	for (auto c : input) {
		if (c == '\n')
			++line;
		if (state == 0) { // start

			if (std::isalpha(c)) {
				cur_word += c;
				state = 1;
			}
			else if (std::isdigit(c)) {
				cur_word += c;
				state = 2;
			}
			else if (IsOperation(c)) {
				cur_word += c;
				state = 5;
			}
			else if (IsPunctuation(c)) {
				cur_word += c;
				res.push_back({ cur_word,{ 5, line } });
				cur_word.clear();
			}
			else if (IsBracket(c)) {
				cur_word += c;
				res.push_back({ cur_word,{ 6, line } });
				cur_word.clear();
			}
			else if (IsSqBracket(c)) {
				cur_word += c;
				res.push_back({ cur_word,{ 7, line } });
				cur_word.clear();
			}
			else if (c == '"') {
				cur_word += c;
				state = 3;
			}
			else if (IsCommentStart(c)) {
				state = 4;
			}
			else if (!IsSpace(c)) {
				throw c;
			}

		}
		else if (state == 1) { // identificator

			if (std::isalpha(c) || std::isdigit(c)) {
				cur_word += c;
			}
			else if (IsOperation(c)) {
				if (!IsReserved(reserved_words, cur_word))
					res.push_back({ cur_word,{ 2, line } });
				else
					res.push_back({ cur_word,{ 1, line } });
				cur_word.clear();
				cur_word += c;
				state = 5;
			}
			else if (IsPunctuation(c)) {
				if (!IsReserved(reserved_words, cur_word))
					res.push_back({ cur_word,{ 2, line } });
				else
					res.push_back({ cur_word,{ 1, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 5, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsBracket(c)) {
				if (!IsReserved(reserved_words, cur_word))
					res.push_back({ cur_word,{ 2, line } });
				else
					res.push_back({ cur_word,{ 1, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 6, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsSqBracket(c)) {
				if (!IsReserved(reserved_words, cur_word))
					res.push_back({ cur_word,{ 2, line } });
				else
					res.push_back({ cur_word,{ 1, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 7, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsSpace(c)) {
				if (!IsReserved(reserved_words, cur_word))
					res.push_back({ cur_word,{ 2, line } });
				else
					res.push_back({ cur_word,{ 1, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsCommentStart(c)) {
				res.push_back({ cur_word,{ 2, line } });
				cur_word.clear();
				state = 4;
			}
			else {
				throw c;
			}

		}
		else if (state == 2) { // literal (number)

			if (std::isdigit(c)) {
				cur_word += c;
			}
			else if (std::isalpha(c)) {
				throw c;
			}
			else if (IsOperation(c)) {
				res.push_back({ cur_word,{ 3, line } });
				cur_word.clear();
				cur_word += c;
				state = 5;
			}
			else if (IsPunctuation(c)) {
				res.push_back({ cur_word,{ 3, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 5, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsBracket(c)) {
				res.push_back({ cur_word,{ 3, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 6, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsSqBracket(c)) {
				res.push_back({ cur_word,{ 3, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 7, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsSpace(c)) {
				res.push_back({ cur_word,{ 3, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsCommentStart(c)) {
				res.push_back({ cur_word,{ 3, line } });
				cur_word.clear();
				state = 4;
			}
			else {
				throw c;
			}

		}
		else if (state == 3) { // literal (string)

			if (std::isdigit(c) || std::isalpha(c) || IsOperation(c) || IsPunctuation(c)
				|| IsBracket(c) || IsSqBracket(c) || c == ' ') {
				cur_word += c;
			}
			else if (c == '"') {
				cur_word += c;
				res.push_back({ cur_word,{ 8, line } });
				cur_word.clear();
				state = 0;
			}
			else {
				throw c;
			}

		}
		else if (state == 4) { // comment

			if (c == '\n') {
				//	res.push_back({ cur_word, 10 });
				cur_word.clear();
				state = 0;
			}
			else {
				cur_word += c;
			}

		}
		else if (state == 5) { // operations (to check for 2-symbol ones like >=, <=, etc.)

			if (IsOperation(c)) {
				cur_word += c;
				if (!IsBigOperation(cur_word)) {
					cur_word.pop_back();
					res.push_back({ cur_word,{ 4, line } });
					cur_word.clear();
					cur_word += c;
				}
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				state = 0;
			}
			else if (std::isalpha(c)) {
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				cur_word += c;
				state = 1;
			}
			else if (std::isdigit(c)) {
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				cur_word += c;
				state = 2;
			}
			else if (IsPunctuation(c)) {
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 5, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsBracket(c)) {
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 6, line } });
				cur_word.clear();
				state = 0;
			}
			else if (IsSqBracket(c)) {
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				cur_word += c;
				res.push_back({ cur_word,{ 7, line } });
				cur_word.clear();
				state = 0;
			}
			else if (c == '"') {
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				cur_word += c;
				state = 3;
			}
			else if (IsCommentStart(c)) {
				res.push_back({ cur_word,{ 4, line } });
				cur_word.clear();
				state = 4;
			}
			else if (!IsSpace(c)) {
				throw c;
			}

		}

	}

	return res;
}