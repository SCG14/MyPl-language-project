//----------------------------------------------------------------------
// FILE: mypl.cpp
// DATE: Spring 2023
// AUTH: Santiago Calvillo
// DESC: This program allows for multiple options to be entered to output different parts of some text
//----------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <filesystem>
#include "token.h"
#include "lexer.h"
#include "simple_parser.h"
#include "ast.h"
#include "print_visitor.h"
#include "ast_parser.h"
#include "semantic_checker.h"
#include "vm.h"
#include "code_generator.h"

using namespace std;
namespace fs = std::filesystem;
int main(int argc, char *argv[]) {
    string option = "";
    string filename = "";
    // checks if argument was entered for option and/or file
    if (argc >= 2) {
        option = argv[1];
    }
    if (argc == 3) {
        filename = argv[2];
    }
    // normal mode with input in terminal
    if (option == "") {
        cout << "[Normal Mode]" << endl;
        istream *input = &cin;
        Lexer lexer(*input);
        try {
            ASTParser parser(lexer);
            Program p = parser.parse();
            SemanticChecker t;
            p.accept(t);
            VM vm;
            CodeGenerator g(vm);
            p.accept(g);
            vm.run();
        } catch (MyPLException &ex) {
            cerr << ex.what() << endl;
        }

    } else if (option == "--help" || argc > 3) {
        // help option, when command entered, or too many arguments
        cout << "Usage: ./mypl [option] [script-file]" << endl;
        cout << "Options:" << endl;
        cout << "--help prints this message" << endl;
        cout << "--lex displays token information" << endl;
        cout << "--parse checks for syntax errors" << endl;
        cout << "--print pretty prints program" << endl;
        cout << "--csharp pretty prints program (typing on terminal not supported as it has to build new file)" << endl;
        cout << "--check statically checks program" << endl;
        cout << "--ir print intermediate (code) representation" << endl;
    } else if (option == "--lex") {
        // lex option, if filename is provided it will print first
        // char from file, else input will be entered and printed
        cout << "[Lex Mode]" << endl;
        if (filename != "") {
            istream *input = new ifstream(filename);
            if (!input->fail()) {
                Lexer lexer(*input);
                try {
                    Token t = lexer.next_token();
                    cout << to_string(t) << endl;
                    while (t.type() != TokenType::EOS) {
                        t = lexer.next_token();
                        cout << to_string(t) << endl;
                    }
                } catch (MyPLException &ex) {
                    cerr << ex.what() << endl;
                }
            } else
                cout << "fail to open file" << endl;
            input->clear();
        } else {
            istream *input = &cin;
            Lexer lexer(*input);
            try {
                Token t = lexer.next_token();
                cout << to_string(t) << endl;
                while (t.type() != TokenType::EOS) {
                    t = lexer.next_token();
                    cout << to_string(t) << endl;
                }
            } catch (MyPLException &ex) {
                cerr << ex.what() << endl;
            }
        }
    } else if (option == "--parse") {
        // parse option, if filename is provided it will print first two
        // char from file, else input will be entered and the first 2 letters printed
        cout << "[Parse Mode]" << endl;
        if (filename != "") {
            istream *input = new ifstream(filename);
            Lexer lexer(*input);
            if (!input->fail()) {
                try {
                    SimpleParser parser(lexer);
                    parser.parse();
                } catch (MyPLException &ex) {
                    cerr << ex.what() << endl;
                }
            } else
                cout << "fail to open file" << endl;
            input->clear();
        } else {
            istream *input = &cin;
            Lexer lexer(*input);
            try {
                SimpleParser parser(lexer);
                parser.parse();
            } catch (MyPLException &ex) {
                cerr << ex.what() << endl;
            }
        }
    } else if (option == "--print") {
        // print option, if filename is provided it will print first word
        // from file, else input will be entered and the first word printed
        if (filename != "") {
            istream *input = new ifstream(filename);
            Lexer lexer(*input);
            if (!input->fail()) {
                try {
                    ASTParser parser(lexer);
                    Program p = parser.parse();
                    PrintVisitor v(cout);
                    p.accept(v);
                } catch (MyPLException &ex) {
                    cerr << ex.what() << endl;
                }
            } else
                cout << "fail to open file" << endl;
            input->clear();
        } else {
            istream *input = &cin;
            Lexer lexer(*input);
            try {
                ASTParser parser(lexer);
                Program p = parser.parse();
                PrintVisitor v(cout);
                p.accept(v);
            } catch (MyPLException &ex) {
                cerr << ex.what() << endl;
            }
        }
    } else if (option == "--check") {
        // check option, if filename is provided it will print first line from file,
        // else input will be entered and print the first line
        cout << "[Check Mode]" << endl;
        if (filename != "") {
            istream *input = new ifstream(filename);
            Lexer lexer(*input);
            if (!input->fail()) {
                try {
                    ASTParser parser(lexer);
                    Program p = parser.parse();
                    SemanticChecker v;
                    p.accept(v);
                } catch (MyPLException &ex) {
                    cerr << ex.what() << endl;
                }

            } else
                cout << "fail to open file" << endl;
            input->clear();
        } else {
            istream *input = &cin;
            Lexer lexer(*input);
            try {
                ASTParser parser(lexer);
                Program p = parser.parse();
                SemanticChecker v;
                p.accept(v);
            } catch (MyPLException &ex) {
                cerr << ex.what() << endl;
            }
        }
    } else if (option == "--ir") {
        // ir option, if filename is provided it will print first two lines
        // from file, else input will be entered and print after 2 lines are entered
        cout << "[IR Mode]" << endl;
        if (filename != "") {
            istream *input = new ifstream(filename);
            Lexer lexer(*input);
            try {
                ASTParser parser(lexer);
                Program p = parser.parse();
                SemanticChecker t;
                p.accept(t);
                VM vm;
                CodeGenerator g(vm);
                p.accept(g);
                cout << to_string(vm) << endl;
            } catch (MyPLException &ex) {
                cerr << ex.what() << endl;
            }
        } else {
            istream *input = &cin;
            Lexer lexer(*input);
            try {
                ASTParser parser(lexer);
                Program p = parser.parse();
                SemanticChecker t;
                p.accept(t);
                VM vm;
                CodeGenerator g(vm);
                p.accept(g);
                cout << to_string(vm) << endl;
            } catch (MyPLException &ex) {
                cerr << ex.what() << endl;
            }
        }
    }
    else if (option == "--csharp") {
        cout << "[C# Mode]" << endl;
        string name = "";
        int i = 0;
        while (filename[i] != '.') {
            name += filename[i];
            i++;
        }
        string dir_name = "C#TestOutputs/c#_" + name;
        istream *input = new ifstream(filename);
        Lexer lexer(*input);

        name += ".cs";
        if (!input->fail()) {
            try {
                ASTParser parser(lexer);
                Program p = parser.parse();
                CSharpPrintVisitor v(name);
                p.accept(v);
            } catch (MyPLException &ex) {
                cerr << ex.what() << endl;
            }
        } else
            cout << "fail to open file " << endl;
        input->clear();

        string cs_file = name;

        // Create directory

        fs::create_directory(dir_name);

        // Copy C# file to directory
        fs::copy_file(cs_file, dir_name + "/" + cs_file, fs::copy_options::overwrite_existing);
        string instr = "rm " + name;
        system(instr.c_str());
        // Change to directory
        if (chdir(dir_name.c_str()) != 0) {
            cerr << "Error changing directory" << endl;
            exit(EXIT_FAILURE);
        }

        system("dotnet new console --force");
        system("rm Program.cs");
        // Run C# file with dotnet
        system("dotnet run");

    }else {
        // if doesn't match any option, first input will be treated as filename to
        // perform normal option
        cout << "[Normal Mode]" << endl;
        filename = option;

        istream *input = new ifstream(filename);
        Lexer lexer(*input);
        try {
            ASTParser parser(lexer);
            Program p = parser.parse();
            SemanticChecker t;
            p.accept(t);
            VM vm;
            CodeGenerator g(vm);
            p.accept(g);
            vm.run();
        } catch (MyPLException &ex) {
            cerr << ex.what() << endl;
        }

    }
}

