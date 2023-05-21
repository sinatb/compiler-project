# Compiler Project - Phase 1

This project has modified and extended the code in [this LLVM tutorial](https://github.com/PacktPublishing/Learn-LLVM-12/tree/master/Chapter03/calc)

## Language
You can read the full grammer from [here](https://github.com/sinatb/compiler-project/blob/main/grammer.txt).
It basically has 2 types of instructions:
+ Variable decalration
+ Assignment
```
type int a, b;
a = 2;
b = (a + 5) * 13;
```

### Lexer and Tokens
You can see full list of tokens in [here](https://github.com/sinatb/compiler-project/blob/main/src/Lexer.h). Other than common tokens like identifier, arithmetic operations (like +, -, ...), we have added a '=' token to recognize assignments, and also a ';' token to detect end of an instruction.
#### Keywords
- type
- int

### Parser
This compiler uses recursive descent as the main algorithm for parsing. For each non-terminal in the grammer, there exists a function that reads from the token stream and build the corresponding part of the abstract syntax tree (AST).  
The parsing is done according to the grammer specification.

### AST
These are the node classes in the abstract syntax tree.
#### `Instructions`
The main program or the start symbol in the grammer, is a list of two types of instructions.
#### `Instruction`
A superclass for instruction types.
#### `Assign`
A node for an assignment instruction, containing the identifier and the assigned expression.
#### `TypeDecl`
A node for a variable declaration instruction, containing the list of the declared identifiers.
#### `Expr`
A superclass for expressions.
#### `Factor`
A type of `Expr` which has to kinds: identifier and literal number.
#### `BinaryOp`
A type of `Expr` which denotes a binary operation with a left and a right hand side expression as its operands.
#### `AST`
Superclass for every node type. This class implements the tree traversal visitor pattern.

### Semantic Analysis
The tree traversal is done using visitor pattern; To look for a specific type of node, the nodes must accept and propagate the visitor down the tree to reach the needed length; For example the mentioned `DivZeroCheck` analyzer looks for specific `BinaryOp`s and it does not need to look further down to other nodes like `Factor`.
There is two types of semantic analysis in this project:
#### `DeclCheck`
This semantic analyzer traverses the AST and looks for undeclared identfiers or repeatedly declared ones. In its traversal, it uses a set to keep track of the declared identifiers. When found such a identifiers, it emits an error.
#### `DivZeroCheck`
This semantic analyzer traverses the AST to look for `BinaryOp`s that are division and also the right handside is a literal zero. When found such a `BinaryOp` node, it emits a division by zero error.
