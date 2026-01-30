#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

enum class TokenType {
	Number,
	Plus,
	Minus,
	Multiply,
	Divide,
	MoreThan,
	LessThan,
	Print,

	End
};

struct Token {
	TokenType type;
	std::string value;
};

class MLTParser {
	std::string source;
	size_t pos = 0;

public: 
	MLTParser(std::string src) : source(src) {};

	Token GetNextToken() {
		while (pos < source.length() && isspace(source[pos])) pos++;

		if (pos >= source.length()) return { TokenType::End, "" };
		char current = source[pos++];
		if (current >= '0' && current <= '9') {
			std::string num;
			num += current;
			while (pos < source.length() && isdigit(source[pos])) num += source[pos++];
			return { TokenType::Number, num };
		}
		if (isalpha(current)) {
			std::string word;
			word += current;
			while (pos < source.length() && isalnum(source[pos])) word += source[pos++];
			if (word == "print") return { TokenType::Print, word };

			return { TokenType::End, word };
		}
		if (current == '+') return { TokenType::Plus, "+" };
		if (current == '-') return { TokenType::Minus, "-" };
		if (current == '*') return { TokenType::Multiply, "*" };
		if (current == '/') return { TokenType::Divide, "/" };
		if (current == '>') return { TokenType::MoreThan, ">" };
		if (current == '<') return { TokenType::LessThan, "<" };
		return { TokenType::End, "" };
	}

	// Since this is a transpiling language, let's add a TranspileToC method
	std::string TranspileToC();
	std::string TranspileToCSharp();
};


extern "C" {
	_declspec(dllexport) const char* TranspileLine(const char* input, const char* lang) {
		static std::string result;
		result.clear();
		MLTParser parser(input);
		std::string target(lang);
		
			if (target == "c" || target == "cpp") 
			{
				result = parser.TranspileToC();
			}
			else if (target == "csharp" || target == "cs") {
				result = parser.TranspileToCSharp();
			}
			else {
				result = "Unsupported Language";
			}
			return result.c_str();
	}
	_declspec(dllexport) const char* TranspileFile(const char* in,
													const char* out,
													const char* lang) 
	{
		std::ifstream input(in);
		if (!input.is_open()) return "";
		std::ofstream output(out);
		std::string line;
		std::string targetlang(lang);
		if (targetlang == "c") {
			output << "#include <stdio.h>\n\nint main() {\n";
		}
		else if (targetlang == "csharp") {
			output << "using System;\n\nclass Program {\n  static void Main(string[] args) {\n";
		}
		while (std::getline(input, line)) {
			if (line.empty()) continue;
			std::cout << line << "\n";
			const char* result = TranspileLine(line.c_str(), lang);
			output << "	" << result << "\n";
		}
		if (targetlang == "c") {
			output << "	return 0;\n}\n";
		} else if (targetlang == "csharp") {
			output << "}\n}\n";
		}
		return "File transpiled successfully.";
	}
}