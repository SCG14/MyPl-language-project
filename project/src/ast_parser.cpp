//----------------------------------------------------------------------
// FILE: ast_parser.cpp
// DATE: CPSC 326, Spring 2023
// AUTH: Santiago Calvillo
// DESC: implementation of recursive functions that build up an ast
//----------------------------------------------------------------------

#include "ast_parser.h"
#include <iostream>
using namespace std;


ASTParser::ASTParser(const Lexer& a_lexer)
  : lexer {a_lexer}
{}


void ASTParser::advance()
{
  curr_token = lexer.next_token();
}


void ASTParser::eat(TokenType t, const string& msg)
{
  if (!match(t))
    error(msg);
  advance();
}


bool ASTParser::match(TokenType t)
{
  return curr_token.type() == t;
}


bool ASTParser::match(initializer_list<TokenType> types)
{
  for (auto t : types)
    if (match(t))
      return true;
  return false;
}


void ASTParser::error(const string& msg)
{
  string s = msg + " found '" + curr_token.lexeme() + "' ";
  s += "at line " + to_string(curr_token.line()) + ", ";
  s += "column " + to_string(curr_token.column());
  throw MyPLException::ParserError(s);
}


bool ASTParser::bin_op()
{
  return match({TokenType::PLUS, TokenType::MINUS, TokenType::TIMES,
      TokenType::DIVIDE, TokenType::AND, TokenType::OR, TokenType::EQUAL,
      TokenType::LESS, TokenType::GREATER, TokenType::LESS_EQ,
      TokenType::GREATER_EQ, TokenType::NOT_EQUAL});
}


Program ASTParser::parse()
{
  Program p;
  advance();
  while (!match(TokenType::EOS)) {
    if (match(TokenType::STRUCT))
      struct_def(p);
    else
      fun_def(p);
  }
  eat(TokenType::EOS, "expecting end-of-file");
  return p;
}

// function for struct definitions
void ASTParser::struct_def(Program& p)
{
    StructDef structDef;
    advance();
    structDef.struct_name = curr_token;
    eat(TokenType::ID, "error");
    eat(TokenType::LBRACE, "error");
    fields(structDef);
    eat(TokenType::RBRACE, "error");
    p.struct_defs.push_back(structDef);
}

// function for function definitions
void ASTParser::fun_def(Program& p)
{
    FunDef funDef;
    if (match(TokenType::VOID_TYPE)) {
        funDef.return_type.type_name = curr_token.lexeme();
        advance();
    }
    else
        funDef.return_type = data_type();
    funDef.fun_name = curr_token;
    eat(TokenType::ID, "error");
    eat(TokenType::LPAREN, "error");
    params(funDef);
    eat(TokenType::RPAREN, "error");
    eat(TokenType::LBRACE, "error");
    if(!match(TokenType::EOS)) {
        while (!match({TokenType::RBRACE, TokenType::EOS})) {
            stmt(funDef.stmts);
        }
    }
    eat(TokenType::RBRACE, "error");
    p.fun_defs.push_back(funDef);
}


// TODO: Finish rest of parser based on your simple parser
// implementation
// function for fields in struct def
void ASTParser::fields(StructDef& structDef) {
    if(!match(TokenType::RBRACE)) {
        VarDef varDef;
        varDef.data_type = data_type();
        varDef.var_name = curr_token;
        eat(TokenType::ID, "error");
        structDef.fields.push_back(varDef);
        while (match(TokenType::COMMA)) {
            eat(TokenType::COMMA, "error");
            VarDef varDef;
            varDef.data_type = data_type();
            varDef.var_name = curr_token;
            eat(TokenType::ID, "error");
            structDef.fields.push_back(varDef);
        }
    }
}

// returns a data type object to be used
DataType ASTParser::data_type() {
    DataType dataType;
    if(match(TokenType::ID)) {
        dataType.type_name = curr_token.lexeme();
        advance();
    }
    else if(match(TokenType::ARRAY)) {
        dataType.is_array = true;
        advance();
        if(base_type()) {
            dataType.type_name = curr_token.lexeme();
            advance();
        }
        else {
            dataType.type_name = curr_token.lexeme();
            eat(TokenType::ID, "error");
        }
    }
    else if(base_type()) {
        dataType.type_name = curr_token.lexeme();
        advance();
    }
    else
        error("error");
    return dataType;
}

// function for parameters in function definitions
void ASTParser::params(FunDef& funDef) {
    if(!match(TokenType::RPAREN)) {
        VarDef varDef;
        varDef.data_type = data_type();
        varDef.var_name = curr_token;
        eat(TokenType::ID, "error");
        funDef.params.push_back(varDef);
        while (match(TokenType::COMMA)) {
            eat(TokenType::COMMA, "error");
            varDef.data_type = data_type();
            varDef.var_name = curr_token;
            eat(TokenType::ID, "error");
            funDef.params.push_back(varDef);
        }
    }
}

// returns true if it is a base type
bool ASTParser::base_type() {
    return match({TokenType::INT_TYPE, TokenType::DOUBLE_TYPE, TokenType::BOOL_TYPE,
                  TokenType::CHAR_TYPE, TokenType::STRING_TYPE});

}

// depending on the type of statement will execute code for that
void ASTParser::stmt(std::vector<std::shared_ptr<Stmt>>& stmts) {
    if(match(TokenType::RETURN)) {
        std::shared_ptr<ReturnStmt> returnStmt = std::make_shared<ReturnStmt>();
        eat(TokenType::RETURN, "error");
        expr(returnStmt->expr);
        stmts.push_back(returnStmt);
    }
    else if(match(TokenType::IF)) {
        IfStmt ifStmt;
        if_stmt(ifStmt);
        std::shared_ptr<IfStmt> ifPtr = std::make_shared<IfStmt>(ifStmt);
        stmts.push_back(ifPtr);
    }
    else if(match(TokenType::WHILE)){
        WhileStmt whileStmt;
        while_stmt(whileStmt);
        std::shared_ptr<WhileStmt> whilePtr = std::make_shared<WhileStmt>(whileStmt);
        stmts.push_back(whilePtr);
    }
    else if(match(TokenType::FOR)){
        ForStmt forStmt;
        for_stmt(forStmt);
        std::shared_ptr<ForStmt> forPtr = std::make_shared<ForStmt>(forStmt);
        stmts.push_back(forPtr);
    }
    else if(base_type() || match(TokenType::ARRAY)) {
        VarDeclStmt decl;
        vdecl_stmt(decl);
        std::shared_ptr<VarDeclStmt> varDecl = std::make_shared<VarDeclStmt>(decl);
        stmts.push_back(varDecl);
    }
    else if(match(TokenType::ID)) {
        Token tmp = curr_token;
        advance();
        if(match(TokenType::LPAREN)) {
            CallExpr callExpr;
            callExpr.fun_name = tmp;
            call_expr(callExpr);
            std::shared_ptr<CallExpr> callExprPtr = std::make_shared<CallExpr>(callExpr);
            stmts.push_back(callExprPtr);
        } else if(match(TokenType::ID)) {
            VarDeclStmt decl;
            VarDef varDef;
            DataType dataType;
            dataType.type_name = tmp.lexeme();
            varDef.var_name = curr_token;
            varDef.data_type = dataType;
            decl.var_def = varDef;
            vdecl_stmt(decl);
            std::shared_ptr<VarDeclStmt> varDecl = std::make_shared<VarDeclStmt>(decl);
            stmts.push_back(varDecl);
        }
        else {
            AssignStmt decl;
            VarRef ref;
            ref.var_name = tmp;
            if(match(TokenType::LBRACKET)) {
                eat(TokenType::LBRACKET, "error");
                Expr array_expr;
                expr(array_expr);
                std::optional<Expr> opt_expr(array_expr);
                ref.array_expr = opt_expr;
                eat(TokenType::RBRACKET, "error");
            }
            decl.lvalue.push_back(ref);
            assign_stmt(decl);
            std::shared_ptr<AssignStmt> varDecl = std::make_shared<AssignStmt>(decl);
            stmts.push_back(varDecl);
        }
    }
    else
        error("error");
}

// expression function that checks for negated and then determines if it is simple or complex
void ASTParser::expr(Expr& expression) {
    if(match(TokenType::NOT)) {
        std::shared_ptr<SimpleTerm> simpleTerm = std::make_shared<SimpleTerm>();
        expression.negated = true;
        eat(TokenType::NOT, "error");
        expr(expression);
    }
    else if(match(TokenType::LPAREN)) {
        eat(TokenType::LPAREN, "error");
        std::shared_ptr<ComplexTerm> complexTerm = std::make_shared<ComplexTerm>();
        expr(complexTerm->expr);
        expression.first = complexTerm;
        eat(TokenType::RPAREN, "error");
    }
    else {
        SimpleTerm term;
        rvalue(term);
        std::shared_ptr<SimpleTerm> simpleTerm = std::make_shared<SimpleTerm>(term);
        expression.first = simpleTerm;
    }

    if(bin_op()) {
        expression.op = curr_token;
        advance();
        Expr expr1;
        expr(expr1);
        std::shared_ptr<Expr> rest = std::make_shared<Expr>(expr1);
        expression.rest = rest;
    }
}

// function for different r value types like new, base or id
void ASTParser::rvalue(SimpleTerm& term) {
    if (match(TokenType::NEW)) {
        NewRValue newRValue;
        new_rvalue(newRValue);
        std::shared_ptr<NewRValue> newRPtr = std::make_shared<NewRValue>(newRValue);
        term.rvalue = newRPtr;
    }
    else if (base_rvalue()){
        std::shared_ptr<SimpleRValue> simpleRValue = std::make_shared<SimpleRValue>();
        simpleRValue->value = curr_token;
        term.rvalue = simpleRValue;
        advance();
    }
    else if(match(TokenType::ID)){
        Token tmp = curr_token;
        advance();
        if(match(TokenType::LPAREN)) {
            CallExpr callExpr;
            callExpr.fun_name = tmp;
            call_expr(callExpr);
            std::shared_ptr<CallExpr> callExprPtr = std::make_shared<CallExpr>(callExpr);
            term.rvalue = callExprPtr;
        }
        else{
            VarRValue varRValue;
            VarRef ref;
            ref.var_name = tmp;
            if(match(TokenType::LBRACKET)) {
                eat(TokenType::LBRACKET, "error");
                Expr array_expr;
                expr(array_expr);
                std::optional<Expr> opt_expr(array_expr);
                ref.array_expr = opt_expr;
                eat(TokenType::RBRACKET, "error");
            }
            varRValue.path.push_back(ref);
            var_rvalue(varRValue);
            std::shared_ptr<VarRValue> varRPtr = std::make_shared<VarRValue>(varRValue);
            term.rvalue = varRPtr;
        }
    }
    else
        error("error");
}

bool ASTParser::base_rvalue() {
    return match({TokenType::INT_VAL,TokenType::DOUBLE_VAL,TokenType::BOOL_VAL,
                  TokenType::CHAR_VAL,TokenType::STRING_VAL,TokenType::NULL_VAL});
}

// function for variable declaration statements
void ASTParser::vdecl_stmt(VarDeclStmt& decl) {
    VarDef varDef;
    if(!match(TokenType::ID)) {
        varDef.data_type = data_type();
        varDef.var_name = curr_token;
        decl.var_def = varDef;
    }
    eat(TokenType::ID, "error");
    eat(TokenType::ASSIGN, "error");
    expr(decl.expr);
}

// function for assign statements
void ASTParser::assign_stmt(AssignStmt& decl) {
    lvalue(decl.lvalue);
    eat(TokenType::ASSIGN, "error");
    expr(decl.expr);
}

// function if lvalue has dots or brackets
void ASTParser::lvalue(std::vector<VarRef>& values) {
    while (match(TokenType::DOT)) {
        advance();
        VarRef varRef;
        varRef.var_name = curr_token;
        eat(TokenType::ID, "error");
        if(match(TokenType::LBRACKET)) {
            eat(TokenType::LBRACKET, "error");
            Expr array_expr;
            expr(array_expr);
            std::optional<Expr> opt_expr(array_expr);
            varRef.array_expr = opt_expr;
            eat(TokenType::RBRACKET, "error");
        }
        values.push_back(varRef);
    }
}

// function for if statements
void ASTParser::if_stmt(IfStmt& ifStmt) {
    eat(TokenType::IF, "error");
    eat(TokenType::LPAREN, "error");
    BasicIf basicIf;
    expr(basicIf.condition);
    eat(TokenType::RPAREN, "error");
    eat(TokenType::LBRACE, "error");
    while(!match(TokenType::RBRACE))
        stmt(basicIf.stmts);
    ifStmt.if_part = basicIf;
    eat(TokenType::RBRACE, "error");
    if_stmt_t(ifStmt);
}

// function for else ifs or else if exist
void ASTParser::if_stmt_t(IfStmt& ifStmt) {
    if(match(TokenType::ELSEIF)) {
        eat(TokenType::ELSEIF, "error");
        eat(TokenType::LPAREN, "error");
        BasicIf basicIf;
        expr(basicIf.condition);
        eat(TokenType::RPAREN, "error");
        eat(TokenType::LBRACE, "error");
        while(!match(TokenType::RBRACE))
            stmt(basicIf.stmts);
        ifStmt.else_ifs.push_back(basicIf);
        eat(TokenType::RBRACE, "error");
        if_stmt_t(ifStmt);
    }
    else if(match(TokenType::ELSE)) {
        eat(TokenType::ELSE, "error");
        eat(TokenType::LBRACE, "error");
        while(!match(TokenType::RBRACE))
            stmt(ifStmt.else_stmts);
        eat(TokenType::RBRACE, "error");
    }
}

// function for while loop
void ASTParser::while_stmt(WhileStmt& whileStmt) {
    eat(TokenType::WHILE, "error");
    eat(TokenType::LPAREN, "error");
    expr(whileStmt.condition);
    eat(TokenType::RPAREN, "error");
    eat(TokenType::LBRACE, "error");
    while(!match(TokenType::RBRACE))
        stmt(whileStmt.stmts);
    eat(TokenType::RBRACE, "error");
}

// function for when a function is called
void ASTParser::call_expr(CallExpr& callExpr) {
    advance();
    if(!match(TokenType::RPAREN)) {
        Expr expression;
        expr(expression);
        callExpr.args.push_back(expression);
        while(!match(TokenType::RPAREN)) {
            eat(TokenType::COMMA, "error");
            Expr expression;
            expr(expression);
            callExpr.args.push_back(expression);
        }
    }
    eat(TokenType::RPAREN, "error");
}

// function for new value
void ASTParser::new_rvalue(NewRValue& newRValue) {
    eat(TokenType::NEW, "error");
    if(base_type()) {
        newRValue.type = curr_token;
        advance();
        eat(TokenType::LBRACKET, "error");
        Expr array_expr;
        expr(array_expr);
        std::optional<Expr> opt_expr(array_expr);
        newRValue.array_expr = opt_expr;
        eat(TokenType::RBRACKET, "error");
    }
    else if(match(TokenType::ID)) {
        newRValue.type = curr_token;
        advance();
        if(match(TokenType::LBRACKET)) {
            advance();
            Expr array_expr;
            expr(array_expr);
            std::optional<Expr> opt_expr(array_expr);
            newRValue.array_expr = opt_expr;
            eat(TokenType::RBRACKET, "error");
        }
    }
    else
        error("error");
}

// function for r values that have brackets or dots
void ASTParser::var_rvalue(VarRValue& varRValue) {
    while (match(TokenType::DOT)) {
        advance();
        VarRef varRef;
        varRef.var_name = curr_token;
        eat(TokenType::ID, "error");
        if(match(TokenType::LBRACKET)) {
            eat(TokenType::LBRACKET, "error");
            Expr array_expr;
            expr(array_expr);
            std::optional<Expr> opt_expr(array_expr);
            varRef.array_expr = opt_expr;
            eat(TokenType::RBRACKET, "error");
        }
        varRValue.path.push_back(varRef);
    }
}

// function for statements with for loops
void ASTParser::for_stmt(ForStmt& forStmt) {
    eat(TokenType::FOR, "error");
    eat(TokenType::LPAREN, "error");
    VarDeclStmt decl;
    VarDef varDef;
    DataType dataType;
    dataType.type_name = curr_token.lexeme();
    if(match(TokenType::ID))
        advance();
    varDef.var_name = curr_token;
    varDef.data_type = dataType;
    decl.var_def = varDef;
    vdecl_stmt(decl);
    forStmt.var_decl = decl;

    eat(TokenType::SEMICOLON, "error");
    expr(forStmt.condition);
    eat(TokenType::SEMICOLON, "error");
    VarRef ref;
    ref.var_name = curr_token;
    eat(TokenType::ID, "error");

    AssignStmt assignStmt;

    if(match(TokenType::LBRACKET)) {
        eat(TokenType::LBRACKET, "error");
        Expr array_expr;
        expr(array_expr);
        std::optional<Expr> opt_expr(array_expr);
        ref.array_expr = opt_expr;
        eat(TokenType::RBRACKET, "error");
    }
    assignStmt.lvalue.push_back(ref);
    assign_stmt(assignStmt);
    forStmt.assign_stmt = assignStmt;

    eat(TokenType::RPAREN, "error");
    eat(TokenType::LBRACE, "error");
    while(!match(TokenType::RBRACE))
        stmt(forStmt.stmts);
    eat(TokenType::RBRACE, "error");
}