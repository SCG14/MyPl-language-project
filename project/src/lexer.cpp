//----------------------------------------------------------------------
// FILE: lexer.cpp
// DATE: CPSC 326, Spring 2023
// NAME: Santiago Calvillo
// DESC: Lexer that takes an input and creates tokens out of it
//----------------------------------------------------------------------

#include "lexer.h"

using namespace std;


Lexer::Lexer(istream& input_stream)
  : input {input_stream}, column {0}, line {1}
{}


char Lexer::read()
{
  ++column;
  return input.get();
}


char Lexer::peek()
{
  return input.peek();
}


void Lexer::error(const string& msg, int line, int column) const
{
  throw MyPLException::LexerError(msg + " at line " + to_string(line) +
                                  ", column " + to_string(column));
}


Token Lexer::next_token()
{

    bool isComment = false;
    char ch;
    while(true) {
        // gets rid of all the spaces
        while (peek() == ' ') {
            ch = read();
        }
        // checks for new lines and gets rid of those
        if(peek() == '\n' || peek() == '\t' || peek() == '\0')
        {
            read();
            column = 0;
            if(isComment)
                isComment = false;
            line++;
        }
        else if(peek() == '#') {
            // if it's a comment will activate comment bool to avoid reading line
            ch = read();
            isComment = true;
        }
        else if(peek() == '\'' && !isComment) {
            //checks for char, reads and disregards the '
            read();
            // reads char so next should be '
            ch = read();
            //checks for errors or incomplete chars
            if(ch == EOF)
                error("found end-of-file in character", line, column);
            if(ch == '\n')
                error("found end-of-line in character", line, column);
            if(ch == '\'')
                error("empty character", line, column);
            // special case backslash
            if(ch == '\\') {
                if(peek() != 'n' && peek() != 't')
                    error("unvalid character", line, column);
                else {
                    string s = "";
                    s += ch;
                    s += read();
                    if(peek() == '\'') {
                        read();
                        return Token(TokenType::CHAR_VAL, s, line, column-3);
                    }
                    else {
                        string msg = "expecting ' found ";
                        msg += read();
                        error(msg, line, column);
                    }
                }
            }
            if(peek() == '\'') {
                // forms char token
                read();
                string s = "";
                s += ch;
                return Token(TokenType::CHAR_VAL, s, line, column-2);
            }
            else {
                string msg = "expecting ' found ";
                msg += read();
                error(msg, line, column);
            }
        }
        else if(peek() == '\"' && !isComment) {
            // code for strings
            read();
            string s = "";
            int count = 0;
            while(peek() != '\"') {
                // reads everything within paren
                ch = read();
                s += ch;
                count++;
                // checks that string closes
                if(ch == '\n') {
                    error("found end-of-line in string", line, column);
                }
                if(ch == EOF) {
                    error("found end-of-file in string", line, column);
                }
            }
            read();
            count++;
            return Token(TokenType::STRING_VAL, s, line, column-count);

        }
        else if(!isComment) {
            // special case !, should be followed by an =
            ch = read();
            if(ch == '!') {
                if(peek() == '=') {
                    read();
                    return Token(TokenType::NOT_EQUAL, "!=", line, column-1);
                }
                else {
                    string s = "expecting '!=' found '!";
                    s += read();
                    s += "'";
                    error(s, line, column-1);
                }
            }
            if(isdigit(ch)) {
                // number code, first checks if it's 0 and is not followed by a number
                if(ch == '0') {
                    if(isdigit(peek()))
                        error("leading zero in number", line, column);
                }
                string num;
                num += ch;
                int count = 0;
                // if there is decimal then is a double, made for one num then .
                if(peek() == '.') {
                    num += read();
                    count++;
                    // checks it does not terminate at a number., must have another digit
                    if(isdigit(peek())) {
                        while (isdigit(peek())) {
                            num += read();
                            count++;
                        }
                        return Token(TokenType::DOUBLE_VAL, num, line, column - count);
                    }
                    else {
                        string s = "missing digit in '";
                        s += num;
                        s += "'";
                        error(s, line, column);
                    }

                }
                // more decimals before point or no point
                while(isdigit(peek())) {
                    num += read();
                    count++;
                    if(peek() == '.') {
                        num += read();
                        count++;
                        if(isdigit(peek())) {
                            while(isdigit(peek())) {
                                num += read();
                                count++;
                            }
                            return Token(TokenType::DOUBLE_VAL, num, line, column-count);
                        }
                        else {
                            read();
                            string s = "missing digit in '";
                            s += num;
                            s += "'";
                            error(s, line, column);
                        }
                    }
                }
                return Token(TokenType::INT_VAL, num, line, column-count);
            }
            // checks for chars
            else if (ch == '.')
                return Token(TokenType::DOT, ".", line, column);
            else if (ch == ',')
                return Token(TokenType::COMMA, ",", line, column);
            else if (ch == ';')
                return Token(TokenType::SEMICOLON, ";", line, column);
            else if (ch == '+')
                return Token(TokenType::PLUS, "+", line, column);
            else if (ch == '-')
                return Token(TokenType::MINUS, "-", line, column);
            else if (ch == '*')
                return Token(TokenType::TIMES, "*", line, column);
            else if (ch == '(')
                return Token(TokenType::LPAREN, "(", line, column);
            else if (ch == ')')
                return Token(TokenType::RPAREN, ")", line, column);
            else if (ch == '{')
                return Token(TokenType::LBRACE, "{", line, column);
            else if (ch == '}')
                return Token(TokenType::RBRACE, "}", line, column);
            else if (ch == '<')
                // less eq
                if(peek() == '=') {
                    read();
                    return Token(TokenType::LESS_EQ, "<=", line, column-1);
                } else
                    return Token(TokenType::LESS, "<", line, column);
            else if (ch == '>')
                if(peek() == '=') {
                    read();
                    return Token(TokenType::GREATER_EQ, ">=", line, column-1);
                } else
                    return Token(TokenType::GREATER, ">", line, column);
            else if (ch == '=') {
                if(peek() == '=') {
                    read();
                    return Token(TokenType::EQUAL, "==", line, column-1);
                } else
                    return Token(TokenType::ASSIGN, "=", line, column);
            }
            else if (ch == '[')
                return Token(TokenType::LBRACKET, "[", line, column);
            else if (ch == ']')
                return Token(TokenType::RBRACKET, "]", line, column);
            else if (ch == '/')
                return Token(TokenType::DIVIDE, "/", line, column);

            // if is a letter will build word
            else if(isalpha(ch)) {
                string word = "";
                word += ch;
                int count = 0;
                // read until character is different from those allowed
                while(isalpha(peek()) || isdigit(peek()) || peek() == '_') {
                    word += read();
                    count++;
                }
                // compares with reserved words
                if(word == "null")
                    return Token(TokenType::NULL_VAL, word, line, column-count);
                else if(word == "true")
                    return Token(TokenType::BOOL_VAL, word, line, column-count);
                else if(word == "false")
                    return Token(TokenType::BOOL_VAL, word, line, column-count);
                else if(word == "int")
                    return Token(TokenType::INT_TYPE, word, line, column-count);
                else if(word == "double")
                    return Token(TokenType::DOUBLE_TYPE, word, line, column-count);
                else if(word == "char")
                    return Token(TokenType::CHAR_TYPE, word, line, column-count);
                else if(word == "string")
                    return Token(TokenType::STRING_TYPE, word, line, column-count);
                else if(word == "bool")
                    return Token(TokenType::BOOL_TYPE, word, line, column-count);
                else if(word == "void")
                    return Token(TokenType::VOID_TYPE, word, line, column-count);
                else if(word == "and")
                    return Token(TokenType::AND, word, line, column-count);
                else if(word == "or")
                    return Token(TokenType::OR, word, line, column-count);
                else if(word == "not")
                    return Token(TokenType::NOT, word, line, column-count);
                else if(word == "if")
                    return Token(TokenType::IF, word, line, column-count);
                else if(word == "elseif")
                    return Token(TokenType::ELSEIF, word, line, column-count);
                else if(word == "else")
                    return Token(TokenType::ELSE, word, line, column-count);
                else if(word == "for")
                    return Token(TokenType::FOR, word, line, column-count);
                else if(word == "while")
                    return Token(TokenType::WHILE, word, line, column-count);
                else if(word == "return")
                    return Token(TokenType::RETURN, word, line, column-count);
                else if(word == "struct")
                    return Token(TokenType::STRUCT, word, line, column-count);
                else if(word == "array")
                    return Token(TokenType::ARRAY, word, line, column-count);
                else if(word == "new")
                    return Token(TokenType::NEW, word, line, column-count);
                else {
                    // if not a reserved word then is an ID
                    return Token(TokenType::ID, word, line, column-count);
                }

            }
            else if(ch == EOF)
                // if end of file end
                return Token(TokenType::EOS, "end-of-stream", line, column);
            else {
                // if it did not belong to a category then is not allowed
                string s = "unexpected character '";
                s += ch;
                s += "'";
                error(s, line, column);
            }
        }
        else {
            // ends and reads comment line
            while(peek() != EOF && peek() != '\n')
                read();
            if(peek() == EOF) {
                read();
                return Token(TokenType::EOS, "end-of-stream", line, column);
            }
            if(peek() == '\n' || peek() != '\t' || peek() != '\0') {
                read();
                line++;
                isComment = false;
                column = 0;
            }
        }

    }
}
  

