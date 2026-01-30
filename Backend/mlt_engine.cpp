#include "mlt_engine.h"
#include <iostream>

std::string MLTParser::TranspileToC() {
	Token t1 = GetNextToken();
	if (t1.type == TokenType::Print) {
		Token target = GetNextToken();
		if (target.type == TokenType::Number) return "printf(\"%d\\n\", " + target.value + ");\n";
		else return "printf(\"%s\\n\", " + target.value + ");\n";
	}
	Token op = GetNextToken();
	Token t2 = GetNextToken();
	if (op.type == TokenType::Plus) return t1.value + " + " + t2.value + ";";
	if (op.type == TokenType::Minus) return t1.value + " - " + t2.value + ";";
	if (op.type == TokenType::Multiply) return t1.value + " * " + t2.value + ";";
	if (op.type == TokenType::Divide) return t1.value + " / " + t2.value + ";";
	return "";
}

std::string MLTParser::TranspileToCSharp()
{
	Token t1 = GetNextToken();
	if (t1.type == TokenType::Print) {
		Token target = GetNextToken();
		return "System.Console.WriteLine(" + target.value + ");";
	}
	Token op = GetNextToken();
	Token t2 = GetNextToken();
	if (op.type == TokenType::Plus) return t1.value + " + " + t2.value + ";";
	if (op.type == TokenType::Minus) return t1.value + " - " + t2.value + ";";
	if (op.type == TokenType::Multiply) return t1.value + " * " + t2.value + ";";
	if (op.type == TokenType::Divide) return t1.value + " / " + t2.value + ";";
	return "";
}
