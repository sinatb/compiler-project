#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
class DeclCheck : public ASTVisitor {
  llvm::StringSet<> Scope;
  bool HasError;

  enum ErrorType { Twice, Not };

  void error(ErrorType ET, llvm::StringRef V) {
    llvm::errs() << "Variable " << V << " "
                 << (ET == Twice ? "already" : "not")
                 << " declared\n";
    HasError = true;
  }

public:
  DeclCheck() : HasError(false) {}

  bool hasError() { return HasError; }

  virtual void visit(Factor &Node) override {
    if (Node.getKind() == Factor::Ident) {
      if (Scope.find(Node.getVal()) == Scope.end())
        error(Not, Node.getVal());
    }
  };

  virtual void visit(BinaryOp &Node) override {
    if (Node.getLeft())
      Node.getLeft()->accept(*this);
    else
      HasError = true;
    if (Node.getRight())
      Node.getRight()->accept(*this);
    else
      HasError = true;
  };

  virtual void visit(TypeDecl &Node) override {
    for (auto I = Node.begin(), E = Node.end(); I != E;
         ++I) {
      if (!Scope.insert(*I).second)
        error(Twice, *I);
    }
  };

  virtual void visit(Assign &Node) override {
      if (Scope.find(Node.getIdent()) == Scope.end())
        error(Not, Node.getIdent());

      if (Node.getExpr())
        Node.getExpr()->accept(*this);
      else
        HasError = true;
  }

  virtual void visit(Instructions &Node) override {
    for (auto I = Node.begin(), E = Node.end(); I != E; I++) {
      (*I)->accept(*this);
    }
  }
};

class DivZeroCheck : public ASTVisitor {

  bool HasError;

public:

  DivZeroCheck() : HasError(false) {}

  bool hasError() { return HasError; }

  void error() {
    llvm::errs() << "Division by zero.\n";
    HasError = true;
  }

  virtual void visit(BinaryOp &Node) override {
    if (Node.getOperator() == BinaryOp::Div && Node.getRight()->isLiteralZero()) {
      error();
    }
  };

  virtual void visit(Factor &Node) override {
    // ignore
  }

  virtual void visit(Instructions &Node) override {
    for (auto I = Node.begin(), E = Node.end(); I != E; I++) {
      (*I)->accept(*this);
    }
  }

  virtual void visit(TypeDecl &Node) override {
    // ignore
  }

  virtual void visit(Assign &Node) override {
    if (Node.getExpr())
      Node.getExpr()->accept(*this);
    else
      HasError = true;
  }

};
}

bool Sema::semantic(AST *Tree) {
  if (!Tree)
    return false;
  DeclCheck decCheck;
  Tree->accept(decCheck);

  DivZeroCheck divZeroCheck;
  Tree->accept(divZeroCheck);

  return decCheck.hasError() || divZeroCheck.hasError();
}
