This is the directory for my modular Interpreter Project: I will be attempting to containerize every part of 
my interpreter to make integration towards the end of the semester as easy as possible. 

I have organized this project in a standardized format. I have a main.cpp file to organize function processes, 
as well as handle file organization and structure.

CURRENT STAGE: Assignement 4

Comment Removal is complete.

Tokenizer is implemented with a structure that differentiates keywords, procedures, identifiers, and types.

Error Handling is partially integrated—tokenization and parsing errors are collected and logged to errors.txt.

Parser builds a Concrete Syntax Tree (CST) and validates syntactic structure. It supports functions and procedures 
with parameter parsing now.

Output files are only written if no syntax errors are found.

CST output is now also written to a file in the outputfiles directory.

Symbol table integration is also complete. Each function, procedure, parameter, and variable is entered into a 
scoped symbol table.

Asssignment 4 output files are printed as "outputfiles/cst_programming_assignment_3-test_file_X.c"

Directory Structure
-------------------------------------------------------------------------------------------------------------
ModularInterpreter/
│
├── main.cpp                     # Entry point – manages file processing and module calls
│
├── CommentRemover.cpp/.h        # Removes // and /* */ comments from source files
├── Tokenizer.cpp/.h             # Tokenizes clean source into token types
├── ErrorHandler.cpp/.h          # Records and outputs errors from all phases
├── Parser.cpp/.h                # Parses tokens into a CST and validates syntax
├── CSTNode.cpp/.h               # Tree node structure for building the CST
├── TokenStream.cpp/.h           # Provides stream-like access to the token list
├── SymbolTable.cpp/.h           # Tracks scope levels, handles array info, outputs parameter lists
│
├── testfiles/
|   ├── depot                    # A placeholder folder for isolating testing files
│   ├── TestFiles1/              # Tests for CommentRemover
│   ├── TestFiles2/              # Tests for Tokenizer
│   ├── TestFiles3/              # Tests for Parser and CST generation
│   └── TestFiles4/              # Tests for Symbol Table generation
│
├── outputfiles/                 # Tokenized output, CST output, and token lists
│
├── errors.txt                   # Log of errors encountered during tokenization/parsing
├── makefile                     # Build automation script
└── README.md                    # Project description (this file)
