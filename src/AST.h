#ifndef AST_H
#define AST_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

class AST;
class Expr;
class Factor;
class BinaryOp;
class Instructions;
class Instruction;
class TypeDecl;
class Assign;

class ASTVisitor {
public:
  virtual void visit(AST &){};
  virtual void visit(Expr &){};
  virtual void visit(Factor &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(Instructions &) = 0;
  virtual void visit(Instruction &) {};
  virtual void visit(TypeDecl &) = 0;
  virtual void visit(Assign &) = 0;
};

class AST {
public:
  virtual ~AST() {}
  virtual void accept(ASTVisitor &V) = 0;
};

class Expr : public AST {
public:
  Expr() {}
};

class Factor : public Expr {
public:
  enum ValueKind { Ident, Number };

private:
  ValueKind Kind;
  llvm::StringRef Val;

public:
  Factor(ValueKind Kind, llvm::StringRef Val)
      : Kind(Kind), Val(Val) {}
  ValueKind getKind() { return Kind; }
  llvm::StringRef getVal() { return Val; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

class BinaryOp : public Expr {
public:
  enum Operator { Plus, Minus, Mul, Div };

private:
  Expr *Left;
  Expr *Right;
  Operator Op;

public:
  BinaryOp(Operator Op, Expr *L, Expr *R)
      : Op(Op), Left(L), Right(R) {}
  Expr *getLeft() { return Left; }
  Expr *getRight() { return Right; }
  Operator getOperator() { return Op; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

class Instruction : public AST {
public:
  Instruction() {}
};

class Instructions : public AST {
  using InstrVector = llvm::SmallVector<Instruction *, 8>;
  InstrVector Instrs;

public:
  Instructions(llvm::SmallVector<Instruction *, 8> Instrs)
      : Instrs(Instrs) {}
  InstrVector::const_iterator begin() { return Instrs.begin(); }
  InstrVector::const_iterator end() { return Instrs.end(); }

  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

class TypeDecl : public Instruction {
  using VarVector = llvm::SmallVector<llvm::StringRef, 8>;
  VarVector Vars;

public:
  TypeDecl(llvm::SmallVector<llvm::StringRef, 8> Vars)
      : Vars(Vars) {}
  VarVector::const_iterator begin() { return Vars.begin(); }
  VarVector::const_iterator end() { return Vars.end(); }

  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};

class Assign : public Instruction {
  llvm::StringRef Ident;
  Expr *E;

public:
  Assign(llvm::StringRef Ident, Expr *E)
      : Ident(Ident), E(E) {}

  llvm::StringRef getIdent() { return Ident; }
  Expr *getExpr() { return E; }
  virtual void accept(ASTVisitor &V) override {
    V.visit(*this);
  }
};
#endif