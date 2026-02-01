#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

enum class TokenType {
	Number,
	String,
	Plus,
	Minus,
	Multiply,
	Divide,
	MoreThan,
	LessThan,
	Print,
	Assign,
	Equal,
	If,
	Else,
	Elif,
	EndBlock,
	Identifier,
	While,
	DoWhile,
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
	
	Token GetNextToken();

	void SetSource(std::string src) {
		source = src;
		pos = 0;
	}

	// Since this is a transpiling language, let's add a TranspileToC method
	// Add more languages
	std::string TranspileToC();
	std::string TranspileToCSharp();
	std::string TranspileToRust();

private:
	std::map < std::string, std::string> symbols; 
	//realized everything needs a pending condition and isdowhile
	std::string pendingCondition = "";
	bool isDoWhile = false;
};


extern "C" {
	_declspec(dllexport) const char* TranspileLine(const char* input, const char* lang, MLTParser& parser) {
		static std::string result;
		result.clear();
		
		std::string target(lang);
		
			if (target == "c" || target == "cpp") 
			{
				result = parser.TranspileToC();
			}
			else if (target == "csharp" || target == "cs") {
				result = parser.TranspileToCSharp();
			}
			else if (target == "rust" || target == "rs") {
				result = parser.TranspileToRust();
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
		// moved parser here so variables can be used, hope it works
		MLTParser parser("");
		std::ifstream input(in);
		if (!input.is_open()) return "";
		std::ofstream output(out);
		std::string line;
		std::string targetlang(lang);
		if (targetlang == "c") {
			output << "#include <stdio.h>\n\nint main() {\n";
		}
		else if (targetlang == "cpp") {
			output << "#include <cstdio>\n\nint main() {\n";
		}
		else if (targetlang == "csharp" || targetlang == "cs") {
			output << "using System;\n\nclass Program {\n  static void Main(string[] args) {\n";
		}
		else if (targetlang == "rust" || targetlang == "rs") {
			output << "fn main() {\n";
		}
		while (std::getline(input, line)) {
			if (line.empty()) continue;
			std::cout << line << "\n";
			parser.SetSource(line);
			const char* result = TranspileLine(line.c_str(), lang, parser);
			output << "	" << result << "\n";
		}
		if (targetlang == "c" || targetlang == "cpp") {
			output << "	return 0;\n}\n";
		} else if (targetlang == "csharp" || targetlang == "cs") {
			output << "}\n}\n";
		}
		else if (targetlang == "rust" || targetlang == "rs") {
			output << "}\n";
		}
		return "File transpiled successfully.";
	}
}