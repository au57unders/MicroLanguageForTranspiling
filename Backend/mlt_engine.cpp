#include "mlt_engine.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

Token MLTParser::GetNextToken()
{

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
		if (word == "if") return { TokenType::If, word };
		if (word == "else") return { TokenType::Else, word };
		if (word == "elif") return { TokenType::Elif, word };
		if (word == "end") return { TokenType::EndBlock, word };
		if (word == "while") return { TokenType::While, word };
		if (word == "do") return { TokenType::DoWhile, word };
		return { TokenType::Identifier, word }; // instead of just ending, just make it return a var, like a real lang
	}
	if (current == '+') return { TokenType::Plus, "+" };
	if (current == '-') return { TokenType::Minus, "-" };
	if (current == '*') return { TokenType::Multiply, "*" };
	if (current == '/') return { TokenType::Divide, "/" };
	if (current == '>') return { TokenType::MoreThan, ">" };
	if (current == '<') return { TokenType::LessThan, "<" };
	if (current == '#') {
		pos = source.length();
		return { TokenType::End, "" };
	}
	if (current == '"') {
		std::string str;
		while (pos < source.length() && source[pos] != '"') str += source[pos++];
		pos++;
		return {TokenType::String, "\"" + str + "\""}; // \ and " confuses me
	}
	if (current == '=') {
		if (pos < source.length() && source[pos] == '=') {
			pos++;
			return { TokenType::Equal, "==" };
		}
		return { TokenType::Assign, "=" };
	}

	return { TokenType::End, "" };
}

// Someone should invest in this project as much as i'm investing in TranspileToC method
std::string MLTParser::TranspileToC() {
	Token t1 = GetNextToken();
	if (t1.type == TokenType::If) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "if (" + Left.value + " " + _op.value + " " + Right.value + ") {\n";
	}
	if (t1.type == TokenType::Elif) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "} else if (" + Left.value + " " + _op.value + " " + Right.value + ") {\n";
	}
	if (t1.type == TokenType::Else) return "} else {\n";
	if (t1.type == TokenType::EndBlock) {
		if (!pendingCondition.empty()) {
			if (isDoWhile) {
				std::string result = "} while (" + pendingCondition + ");\n";
				pendingCondition = "";
				isDoWhile = false;
				return result;
			}
		}
		return "}\n";
	}
	if (t1.type == TokenType::Print) {
		Token target = GetNextToken();
		if (target.type == TokenType::Number) return "printf(\"%d\\n\", " + target.value + ");\n";
		else return "printf(\"%s\\n\", " + target.value + ");\n";
	}
	if (t1.type == TokenType::While) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "while (" + Left.value + " " + _op.value + " " + Right.value + ") {\n";
	}
	if (t1.type == TokenType::DoWhile) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		pendingCondition = Left.value + " " + _op.value + " " + Right.value;
		isDoWhile = true;
		return "do {\n";
	}

	if (t1.type == TokenType::Identifier || t1.type == TokenType::Number) {
		Token op = GetNextToken();
		if (op.type == TokenType::End) return t1.value + ";";
		if (op.type == TokenType::Assign) {
			Token val = GetNextToken();
			Token lookahead = GetNextToken();

			if (lookahead.type == TokenType::Plus || lookahead.type == TokenType::Minus || lookahead.type == TokenType::Multiply || lookahead.type == TokenType::Divide) {
				Token t3 = GetNextToken();
				symbols[t1.value] = "math_result";
				return "double " + t1.value + " = " + val.value + " " + lookahead.value + " " + t3.value + ";";
			}
			if (val.type == TokenType::Number) {
				return "double " + t1.value + " = " + val.value + ";";
			}
			else {
				return "const char* " + t1.value + " = \"" + val.value + "\";";
			}
		}
		Token t2 = GetNextToken();
		return t1.value + " " + op.value + " " + t2.value + ";";
	}
	
	Token op = GetNextToken();
	Token t2 = GetNextToken();
	if (op.type == TokenType::Plus) return t1.value + " + " + t2.value + ";";
	if (op.type == TokenType::Minus) return t1.value + " - " + t2.value + ";";
	if (op.type == TokenType::Multiply) return t1.value + " * " + t2.value + ";";
	if (op.type == TokenType::Divide) return t1.value + " / " + t2.value + ";";
	if (op.type == TokenType::MoreThan) return t1.value + " > " + t2.value + ";";
	if (op.type == TokenType::LessThan) return t1.value + " < " + t2.value + ";";
	
	if (op.type == TokenType::Equal) return t1.value + " == " + t2.value + ";";
	return "";
}

std::string MLTParser::TranspileToCSharp()
{
	Token t1 = GetNextToken();
	if (t1.type == TokenType::If) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "if (" + Left.value + " " + _op.value + " " + Right.value + ") {\n";
	}
	if (t1.type == TokenType::Elif) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "} else if (" + Left.value + " " + _op.value + " " + Right.value + ") {\n";
	}
	if (t1.type == TokenType::Else) return "} else {\n";
	if (t1.type == TokenType::EndBlock) {
		if (!pendingCondition.empty()) {
			if (isDoWhile) {
				std::string result = "} while (" + pendingCondition + ");\n";
				pendingCondition = "";
				isDoWhile = false;
				return result;
			}
		}
		return "}\n";
	}
	if (t1.type == TokenType::While) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "while (" + Left.value + " " + _op.value + " " + Right.value + ") {\n";
	}
	if (t1.type == TokenType::DoWhile) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		pendingCondition = Left.value + " " + _op.value + " " + Right.value;
		isDoWhile = true;
		return "do {\n";
	}
	if (t1.type == TokenType::Identifier || t1.type == TokenType::Number) {
		Token op = GetNextToken();
		if (op.type == TokenType::End) return t1.value + ";";
		if (op.type == TokenType::Assign) {
			Token val = GetNextToken();
			Token lookahead = GetNextToken();

			if (lookahead.type == TokenType::Plus || lookahead.type == TokenType::Minus || lookahead.type == TokenType::Multiply || lookahead.type == TokenType::Divide) {
				Token t3 = GetNextToken();
				symbols[t1.value] = "math_result";
				return "double " + t1.value + " = " + val.value + " " + lookahead.value + " " + t3.value + ";";
			}
			if (val.type == TokenType::Number) {
				return "double " + t1.value + " = " + val.value + ";";
			}
			else {
				return "string " + t1.value + " = \"" + val.value + "\";";
			}
		}
		Token t2 = GetNextToken();
		return t1.value + " " + op.value + " " + t2.value + ";";
	}

	Token op = GetNextToken();
	Token t2 = GetNextToken();
	if (t1.type == TokenType::Print) return "Console.WriteLine(" + op.value + ");";
	if (op.type == TokenType::Plus) return t1.value + " + " + t2.value + ";";
	if (op.type == TokenType::Minus) return t1.value + " - " + t2.value + ";";
	if (op.type == TokenType::Multiply) return t1.value + " * " + t2.value + ";";
	if (op.type == TokenType::Divide) return t1.value + " / " + t2.value + ";";
	if (op.type == TokenType::MoreThan) return t1.value + " > " + t2.value + ";";
	if (op.type == TokenType::LessThan) return t1.value + " < " + t2.value + ";";

	if (op.type == TokenType::Equal) return t1.value + " == " + t2.value + ";";
	return "";
}

std::string MLTParser::TranspileToRust()
{
	Token t1 = GetNextToken();

	if (t1.type == TokenType::If) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "if " + Left.value + " " + _op.value + " " + Right.value + " {\n";
	}
	if (t1.type == TokenType::Elif) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		return "} else if " + Left.value + " " + _op.value + " " + Right.value + " {\n";
	}
	if (t1.type == TokenType::Else) return "} else {\n";

	if (t1.type == TokenType::Print) {
		Token target = GetNextToken();
		return "println!(\"{}\", " + target.value + ");\n";
	}
	// Rewrite the while loop if it's broken/slow or needs to be reworked
	if (t1.type == TokenType::While) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		pendingCondition = Left.value + " " + _op.value + " " + Right.value;
		isDoWhile = false;
		return "loop {\n";
	}
	if (t1.type == TokenType::DoWhile) {
		Token Left = GetNextToken();
		Token _op = GetNextToken();
		Token Right = GetNextToken();
		pendingCondition = Left.value + " " + _op.value + " " + Right.value;
		isDoWhile = true;
		return "loop {\n";
	}
	if (t1.type == TokenType::EndBlock) {
		if (!pendingCondition.empty()) {
			if (isDoWhile) {
				std::string result = "if " + pendingCondition + " {\nbreak;\n}\n";
				pendingCondition = "";
				isDoWhile = false;
				return result;
			}
		}
		return "}\n";
	}
	if (t1.type == TokenType::Identifier || t1.type == TokenType::Number) {
		Token op = GetNextToken();

		if (op.type == TokenType::End) return t1.value + ";";

		if (op.type == TokenType::Assign) {
			Token val = GetNextToken();
			Token lookahead = GetNextToken();

			if (lookahead.type == TokenType::Plus || lookahead.type == TokenType::Minus ||
				lookahead.type == TokenType::Multiply || lookahead.type == TokenType::Divide) {
				Token t3 = GetNextToken();
				return "let " + t1.value + " = " + val.value + " " + lookahead.value + " " + t3.value + ";";
			}
			return "let " + t1.value + " = " + val.value + ";";
		}
		Token t2 = GetNextToken();
		return "let _ = " + t1.value + " " + op.value + " " + t2.value + ";";
	}

	return "";
}
