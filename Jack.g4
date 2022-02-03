grammar Jack;

/* ------------------- */
/* Class Level Grammar */
/* ------------------- */

classDec       : 'class' className (':' className)? '{' classVarDec* subroutineDec* '}';

classVarDec    : CLASSDECORATOR type varName (',' varName)* ';';

type           : VARTYPES
               | className
               | arrayType
               ;

subroutineDec  : SUBROUTINEDECORATOR ('void'|type) subroutineName '(' parameterList ')' subroutineBody;

parameterList  : ((type varName) (',' type varName)*)?;

subroutineBody : '{' varDec* statements '}';

varDec         : 'var' type varName (',' varName)* ';';

arrayType      : VARTYPES '[' INTEGER ']'
               | className '[' INTEGER ']'
               ;

className      : ID;
subroutineName : ID;
varName        : ID;

/* ------------------------ */
/* Statements Level Grammar */
/* ------------------------ */
statements      : statement*;

statement       : letStatement
                | ifStatement
                | whileStatement
                | doStatement
                | returnStatement
                ;

letStatement    : 'let' varName ('[' expression ']')? '=' expression ';';

ifStatement     : 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?;

whileStatement  : 'while' '(' expression ')' '{' statements '}';

doStatement     : 'do' subroutineCall ';';

returnStatement : 'return' expression? ';';

/* ------------------------ */
/* Expression Level Grammar */
/* ------------------------ */

expression      : term (OP term)*
                | castExpression
                ;

castExpression  : 'cast' varName type ';';

term            : INTEGER
                | CHAR
                | STRING
                | KEYWORD
                | varName
                | varName '[' expression ']'
                | varName '.' varName
                | subroutineCall
                | '{' expression '}'
                | UNARYOP term
                ;

subroutineCall  : subroutineName '(' expressionList ')'
                | varName '.' subroutineName '(' expressionList ')'
                ;

expressionList  : (expression (',' expression)*)?;

/* ----------- */
/* Terminators */
/* ----------- */

VARTYPES       : 'int'
               | 'char'
               | 'boolean'
               ;

CLASSDECORATOR : 'static'
               | 'field'
               ;

SUBROUTINEDECORATOR : 'constructor'
                    | 'function'
                    | 'method'
                    ;

OP      : '+'
        | '-'
        | '*'
        | '/'
        | '&'
        | '|'
        | '<'
        | '>'
        | '='
        ;

UNARYOP : '(-)'
        | '~'
        ;

KEYWORD : 'true'
        | 'false'
        | 'null'
        | 'this'
        ;

INTEGER : [0-9]+;
STRING  : '"' ~('\r'|'\n'|'"'|'\'')* '"';
CHAR    : '\'' ~('\r'|'\n'|'"'|'\'') '\'';
ID      : [a-zA-Z_][a-zA-Z_0-9]*;

/* -------------------------- */
/* Skip WhiteSpace & Comments */
/* -------------------------- */

WS           : [ \n\t\r]+ -> skip;
COMMENT      : '/*' .*? '*/' -> skip;
LINE_COMMENT : '//' ~[\r\n]* -> skip;


