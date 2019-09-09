#define TEST

#include "lexer.cpp"
#include "token.cpp"

#include <string>
#include <vector>

struct Test {
	std::string name;
	bool (* test)();
};

bool runTest(Test test) {
	bool result = (*test.test)();

	if (result) std::cout << test.name << ": PASS" << std::endl;
	return result;
}

void reportMismatch(std::string testName, std::string expected, uint64_t found) {
	std::cout << testName << ": FAILED" << std::endl;
	std::cout << "  expected " << expected << " but found " 
	          << found << std::endl;
}

void reportMismatch(std::string testName,
                    std::string expected,
					std::string found) {
	std::cout << testName << ": FAILED" << std::endl;
	std::cout << "  expected " << expected << " but found " 
	          << found << std::endl;
}

// Whitespace skipping lexing test.
bool testLexerWhitespace() {
	std::string source = "\n   \t\t  \r\n  \r\r\t  \n   \t";
	Lexer lexer(source);

	lexer.scan();

	// Check results.
	if (lexer.getLine() != 4) {
		reportMismatch("Lexer whitespace", "4", lexer.getLine());
		return false;
	}

	if (lexer.tokens.size() != 1) {
		reportMismatch("Lexer whitespace", "1", lexer.tokens.size());
		return false;
	}

	std::string lexeme = "";
	Token t(Token::Type::END, lexeme, 4);
	if (lexer.tokens[0] != t) {
		reportMismatch("Lexer whitespace",
		               t.toString(),
					   lexer.tokens[0].toString());
		return false;
	}

	return true;
}

// Test colon tokenization.
bool testLexerColon() {
	std::string source = "::";
	Lexer lexer(source);

	lexer.scan();

	// Check results.
	if (lexer.tokens.size() != 3) {
		reportMismatch("Lexer colon", "3", lexer.tokens.size());
		return false;
	}

	std::string lexeme = ":";
	Token t(Token::Type::COLON, lexeme, 1);

	if (lexer.tokens[0] != t) {
		reportMismatch("Lexer colon",
		               t.toString(),
					   lexer.tokens[0].toString());
		return false;
	}

	if (lexer.tokens[1] != t) {
		reportMismatch("Lexer colon",
		               t.toString(),
					   lexer.tokens[1].toString());
		return false;
	}

	return true;
}

int main() {
	bool allPass = true;
	std::vector<Test> lexerTests = {
		{ "Lexer whitespace", &testLexerWhitespace },
		{ "Lexer colon",      &testLexerColon      },
	};

	for (auto &test: lexerTests) allPass = runTest(test);

	if (allPass) return 0;
	return 1;
}
