#include "Parser.h"

AST *Parser::parse() {
  AST *Res = parseGoal();
  expect(Token::eoi);
  return Res;
}

Instructions *Parser::parseGoal() {
  llvm::SmallVector<Instruction *, 8> Instrs;

  while (!Tok.is(Token::eoi))
  {
    Instruction *I = parseInstruction();
    Instrs.push_back(I);
  }

  return new Instructions(Instrs);

}

Instruction *Parser::parseInstruction() {
  if (Tok.is(Token::KW_type)) {
    return parseDeclaration();
  }

  return parseAssignment();

}

Assign *Parser::parseAssignment() {
  if (expect(Token::ident))
    return nullptr;
  
  llvm::StringRef identifier = Tok.getText();
  advance();

  if (consume(Token::equal))
    return nullptr;
  
  Expr *E = parseExpr();

  if (consume(Token::semi_colon))
    return nullptr;
  
  return new Assign(identifier, E);
}

TypeDecl *Parser::parseDeclaration() {
  if (consume(Token::KW_type))
    return nullptr;

  if (expect(Token::KW_int))
    return nullptr;
  
  advance();
  if (expect(Token::ident))
    return nullptr;
  
  llvm::SmallVector<llvm::StringRef, 8> Vars;
  Vars.push_back(Tok.getText());
  advance();
  while (Tok.is(Token::comma)) {
    advance();
    if (expect(Token::ident))
      return nullptr;
    Vars.push_back(Tok.getText());
    advance();
  }
  if (consume(Token::semi_colon))
    return nullptr;

  return new TypeDecl(Vars);
}

Expr *Parser::parseExpr() {
  Expr *Left = parseTerm();
  while (Tok.isOneOf(Token::plus, Token::minus)) {
    BinaryOp::Operator Op = Tok.is(Token::plus)
                                ? BinaryOp::Plus
                                : BinaryOp::Minus;
    advance();
    Expr *Right = parseTerm();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

Expr *Parser::parseTerm() {
  Expr *Left = parseFactor();
  while (Tok.isOneOf(Token::star, Token::slash)) {
    BinaryOp::Operator Op =
        Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
    advance();
    Expr *Right = parseFactor();
    Left = new BinaryOp(Op, Left, Right);
  }
  return Left;
}

Expr *Parser::parseFactor() {
  Expr *Res = nullptr;
  switch (Tok.getKind()) {
  case Token::number:
    Res = new Factor(Factor::Number, Tok.getText());
    advance(); break;
  case Token::ident:
    Res = new Factor(Factor::Ident, Tok.getText());
    advance(); break;
  case Token::l_paren:
    advance();
    Res = parseExpr();
    if (!consume(Token::r_paren)) break;
  default:
    if (!Res)
      error();
    while (!Tok.isOneOf(Token::r_paren, Token::star,
                        Token::plus, Token::minus,
                        Token::slash, Token::eoi))
      advance();
  }
  return Res;
}
