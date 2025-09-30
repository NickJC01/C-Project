Token list:

Token type: TYPE
Token: char

Token type: IDENTIFIER
Token: announcement

Token type: L_BRACKET
Token: [

Token type: INTEGER
Token: 2048

Token type: R_BRACKET
Token: ]

Token type: SEMICOLON
Token: ;

Token type: PROCEDURE
Token: procedure

Token type: IDENTIFIER
Token: main

Token type: L_PAREN
Token: (

Token type: TYPE
Token: void

Token type: R_PAREN
Token: )

Token type: L_BRACE
Token: {

Token type: TYPE
Token: char

Token type: IDENTIFIER
Token: name

Token type: L_BRACKET
Token: [

Token type: INTEGER
Token: 100

Token type: R_BRACKET
Token: ]

Token type: SEMICOLON
Token: ;

Token type: IDENTIFIER
Token: name

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: STRING
Token: "Robert\x0"

Token type: SEMICOLON
Token: ;

Token type: IDENTIFIER
Token: announcement

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: STRING
Token: "You've got mail!\x0"

Token type: SEMICOLON
Token: ;

Token type: IDENTIFIER
Token: display_announcement

Token type: L_PAREN
Token: (

Token type: IDENTIFIER
Token: name

Token type: L_BRACKET
Token: [

Token type: INTEGER
Token: 0

Token type: R_BRACKET
Token: ]

Token type: R_PAREN
Token: )

Token type: SEMICOLON
Token: ;

Token type: R_BRACE
Token: }

Token type: UNKNOWN
Token: function

Token type: TYPE
Token: bool

Token type: IDENTIFIER
Token: empty_string

Token type: L_PAREN
Token: (

Token type: TYPE
Token: char

Token type: IDENTIFIER
Token: string

Token type: L_BRACKET
Token: [

Token type: INTEGER
Token: 4096

Token type: R_BRACKET
Token: ]

Token type: R_PAREN
Token: )

Token type: L_BRACE
Token: {

Token type: TYPE
Token: int

Token type: IDENTIFIER
Token: i

Token type: SEMICOLON
Token: ;

Token type: TYPE
Token: int

Token type: IDENTIFIER
Token: num_bytes_before_null

Token type: SEMICOLON
Token: ;

Token type: TYPE
Token: bool

Token type: IDENTIFIER
Token: found_null

Token type: SEMICOLON
Token: ;

Token type: IDENTIFIER
Token: found_null

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: IDENTIFIER
Token: FALSE

Token type: SEMICOLON
Token: ;

Token type: IDENTIFIER
Token: num_bytes_before_null

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: INTEGER
Token: 0

Token type: SEMICOLON
Token: ;

Token type: IDENTIFIER
Token: i

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: INTEGER
Token: 0

Token type: SEMICOLON
Token: ;

Token type: KEYWORD
Token: while

Token type: L_PAREN
Token: (

Token type: L_PAREN
Token: (

Token type: IDENTIFIER
Token: i

Token type: LT
Token: <

Token type: INTEGER
Token: 4096

Token type: R_PAREN
Token: )

Token type: BOOLEAN_AND
Token: &&

Token type: L_PAREN
Token: (

Token type: BOOLEAN_NOT
Token: !

Token type: IDENTIFIER
Token: found_null

Token type: R_PAREN
Token: )

Token type: R_PAREN
Token: )

Token type: L_BRACE
Token: {

Token type: KEYWORD
Token: if

Token type: L_PAREN
Token: (

Token type: IDENTIFIER
Token: string

Token type: L_BRACKET
Token: [

Token type: IDENTIFIER
Token: i

Token type: R_BRACKET
Token: ]

Token type: BOOLEAN_EQUAL
Token: ==

Token type: UNKNOWN
Token: '\x0'

Token type: R_PAREN
Token: )

Token type: L_BRACE
Token: {

Token type: IDENTIFIER
Token: found_null

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: IDENTIFIER
Token: TRUE

Token type: SEMICOLON
Token: ;

Token type: R_BRACE
Token: }

Token type: KEYWORD
Token: else

Token type: L_BRACE
Token: {

Token type: IDENTIFIER
Token: num_bytes_before_null

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: IDENTIFIER
Token: num_bytes_before_null

Token type: PLUS
Token: +

Token type: INTEGER
Token: 1

Token type: SEMICOLON
Token: ;

Token type: R_BRACE
Token: }

Token type: IDENTIFIER
Token: i

Token type: ASSIGNMENT_OPERATOR
Token: =

Token type: IDENTIFIER
Token: i

Token type: PLUS
Token: +

Token type: INTEGER
Token: 1

Token type: SEMICOLON
Token: ;

Token type: R_BRACE
Token: }

Token type: KEYWORD
Token: return

Token type: L_PAREN
Token: (

Token type: IDENTIFIER
Token: num_bytes_before_null

Token type: BOOLEAN_EQUAL
Token: ==

Token type: INTEGER
Token: 0

Token type: R_PAREN
Token: )

Token type: SEMICOLON
Token: ;

Token type: R_BRACE
Token: }

Token type: PROCEDURE
Token: procedure

Token type: IDENTIFIER
Token: display_announcement

Token type: L_PAREN
Token: (

Token type: TYPE
Token: char

Token type: IDENTIFIER
Token: name

Token type: L_BRACKET
Token: [

Token type: INTEGER
Token: 512

Token type: R_BRACKET
Token: ]

Token type: R_PAREN
Token: )

Token type: L_BRACE
Token: {

Token type: KEYWORD
Token: if

Token type: L_PAREN
Token: (

Token type: BOOLEAN_NOT
Token: !

Token type: IDENTIFIER
Token: empty_string

Token type: L_PAREN
Token: (

Token type: IDENTIFIER
Token: name

Token type: R_PAREN
Token: )

Token type: R_PAREN
Token: )

Token type: L_BRACE
Token: {

Token type: IDENTIFIER
Token: printf

Token type: L_PAREN
Token: (

Token type: STRING
Token: "Welcome, %s\n\n"

Token type: COMMA
Token: ,

Token type: IDENTIFIER
Token: name

Token type: R_PAREN
Token: )

Token type: SEMICOLON
Token: ;

Token type: KEYWORD
Token: if

Token type: L_PAREN
Token: (

Token type: BOOLEAN_NOT
Token: !

Token type: IDENTIFIER
Token: empty_string

Token type: L_PAREN
Token: (

Token type: IDENTIFIER
Token: announcement

Token type: R_PAREN
Token: )

Token type: R_PAREN
Token: )

Token type: L_BRACE
Token: {

Token type: IDENTIFIER
Token: printf

Token type: L_PAREN
Token: (

Token type: STRING
Token: "%s\n"

Token type: COMMA
Token: ,

Token type: IDENTIFIER
Token: announcement

Token type: R_PAREN
Token: )

Token type: SEMICOLON
Token: ;

Token type: R_BRACE
Token: }

Token type: R_BRACE
Token: }

Token type: R_BRACE
Token: }

