grammar LamaLanguage;

@parser::header
{
// DO NOT MODIFY - generated from LamaLanguage.g4
import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.source.Source;
import ru.mkn.lama.LamaLanguage;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.nodes.LamaRootNode;
import ru.mkn.lama.nodes.expr.LamaExpressionListNode;
import ru.mkn.lama.parser.LamaNodeFactory;
import ru.mkn.lama.parser.LamaParseError;
}

@lexer::header
{
// DO NOT MODIFY - generated from LamaLanguage.g4
}

@parser::members
{
private LamaNodeFactory factory;
private Source source;

private static final class BailoutErrorListener extends BaseErrorListener {
    private final Source source;
    BailoutErrorListener(Source source) {
        this.source = source;
    }
    @Override
    public void syntaxError(Recognizer<?, ?> recognizer, Object offendingSymbol, int line, int charPositionInLine, String msg, RecognitionException e) {
        throwParseError(source, line, charPositionInLine, (Token) offendingSymbol, msg);
    }
}

public void SemErr(Token token, String message) {
    assert token != null;
    throwParseError(source, token.getLine(), token.getCharPositionInLine(), token, message);
}

private static void throwParseError(Source source, int line, int charPositionInLine, Token token, String message) {
    int col = charPositionInLine + 1;
    String location = "-- line " + line + " col " + col + ": ";
    int length = token == null ? 1 : Math.max(token.getStopIndex() - token.getStartIndex(), 0);
    throw new LamaParseError(source, line, col, length, String.format("Error(s) parsing script:%n" + location + message));
}

public static LamaRootNode parseLama(LamaLanguage language, Source source) {
    LamaLanguageLexer lexer = new LamaLanguageLexer(CharStreams.fromString(source.getCharacters().toString()));
    LamaLanguageParser parser = new LamaLanguageParser(new CommonTokenStream(lexer));
    lexer.removeErrorListeners();
    parser.removeErrorListeners();
    BailoutErrorListener listener = new BailoutErrorListener(source);
    lexer.addErrorListener(listener);
    parser.addErrorListener(listener);
    parser.factory = new LamaNodeFactory(language, source);
    parser.source = source;
    parser.compilationUnit();
    return parser.factory.getRootNode();
}
}

// Parser

//compilationUnit : (importExpression)* scopeExpression EOF;
//importExpression : 'import' UIDENT ';'

compilationUnit
: scopeExpression { factory.setMainBody($scopeExpression.result); }
EOF;

scopeExpression returns [LamaExpressionListNode result]
: {
    factory.startScope();
  }
(definition)* { LamaExpressionListNode expr = null; }
    (expression { expr = $expression.result; }
    )? { $result = factory.finishScope(expr, false); };

expandedScopeExpression returns [LamaExpressionListNode result]
: {
    factory.startScope();
  }
(definition)* { LamaExpressionListNode expr = null; }
    (expression { expr = $expression.result; }
    )? { $result = factory.finishScope(expr, true); };

definition
:
    variableDefinition
    | functionDefinition;

variableDefinition :
'var' variableDefinitionItem (',' variableDefinitionItem)* ';';

variableDefinitionItem :
LIDENT ('=' binaryExpression)? { factory.addVariableDefinition($LIDENT, $binaryExpression.ctx != null ? $binaryExpression.result : null); };

functionDefinition :
'fun' name=LIDENT '(' args=functionArguments ')'
 { factory.enterFunction($args.result); }
 body=functionBody
 { factory.leaveFunction($name, $body.result); };

functionArguments returns [List<Token> result]:
{ List<Token> args = new ArrayList<>(); }
(
    LIDENT { args.add($LIDENT); }
    (',' LIDENT { args.add($LIDENT); })*
)?
{ $result = args; };

functionBody returns [LamaNode result]:
'{' scopeExpression '}' { $result = $scopeExpression.result; };

expression returns [LamaExpressionListNode result]
: { List<LamaNode> body = new ArrayList<>(); }
 (
  binaryExpression ';' { body.add($binaryExpression.result); }
  )*
  binaryExpression  { body.add($binaryExpression.result); }
 { $result = factory.createExpression(body); };
binaryExpression returns [LamaNode result]
:
     binaryOperand {$result = $binaryOperand.result; }
    | op=MINUS e=binaryExpression {$result = factory.createUnaryExpression($op, $e.result); }
    | l=binaryExpression op=(MUL|DIV|MOD) r=binaryExpression { $result = factory.createBinaryExpression($op, $l.result, $r.result); }
    | l=binaryExpression op=(PLUS|MINUS) r=binaryExpression { $result = factory.createBinaryExpression($op, $l.result, $r.result); }
    | l=binaryExpression op=(EQ|NE|LE|LT|GE|GT) r=binaryExpression { $result = factory.createBinaryExpression($op, $l.result, $r.result); }
    | l=binaryExpression op='&&' r=binaryExpression { $result = factory.createBinaryExpression($op, $l.result, $r.result); }
    | l=binaryExpression op='!!' r=binaryExpression { $result = factory.createBinaryExpression($op, $l.result, $r.result); }
//    | <assoc=right> binaryOperand ':' binaryOperand
    | <assoc=right> l=binaryExpression ':=' r=binaryExpression { $result = factory.createAssignment($l.result, $r.result); };
binaryOperand returns [LamaNode result]:
    primary { $result = $primary.result; }
    | name=binaryOperand
     '(' { List<LamaNode> parameters = new ArrayList<>(); }
     (expression { parameters.add($expression.result); }
        (',' expression { parameters.add($expression.result); })*)?
     ')' { $result = factory.createCallExpression($name.text, $name.result, parameters); };
//    | binaryOperand '[' expression ']';
primary returns [LamaNode result]:
    DECIMAL  { $result = factory.createIntLiteral($DECIMAL); }
    | STRING { $result = factory.createStringLiteral($STRING); }
//    | CHAR
    | LIDENT { $result = factory.createIdentifier($LIDENT); }
//    | 'true'
//    | 'false'
    | 'skip' { $result = factory.createSkipExpression(); }
    | '(' scopeExpression ')' { $result = $scopeExpression.result; }
//    | listExpression
//    | arrayExpression
//    | sExpression
    | ifExpression      { $result = $ifExpression.result; }
    | whileDoExpression { $result = $whileDoExpression.result; }
    | doWhileExpression { $result = $doWhileExpression.result; }
    | forExpression     { $result = $forExpression.result; }
//    | caseExpression
;

arrayExpression : '[' (expression (',' expression)*)? ']';
listExpression : '{' (expression (',' expression)*)? '}';
sExpression : UIDENT ('(' (expression (',' expression)*)? ')')?;

ifExpression returns [LamaNode result]:
'if' cond=expression 'then' body=scopeExpression (elsePart)?
'fi' { $result = factory.createIfExpression($cond.result, $body.result, $elsePart.ctx == null ? null : $elsePart.result); }
;
elsePart returns [LamaNode result]:
    'elif' cond=expression 'then' body=scopeExpression (elsePart)?
    { $result = factory.createIfExpression($cond.result, $body.result, $elsePart.ctx == null ? null : $elsePart.result); }
    | 'else' body=scopeExpression { $result = $body.result; }
;

whileDoExpression returns [LamaNode result]:
'while' cond=expression 'do' body=scopeExpression 'od' { $result = factory.createWhileExpression($cond.result, $body.result); };
doWhileExpression returns [LamaNode result]:
'do' body=scopeExpression 'while' cond=expression 'od' { $result = factory.createDoWhileExpression($cond.result, $body.result); };
forExpression returns [LamaNode result]:
'for' before=expandedScopeExpression ',' cond=expression ',' step=expression 'do' body=scopeExpression 'od'
    { $result = factory.createForExpression($before.result, $cond.result, $step.result, $body.result); };

pattern :
    DECIMAL
    | '_'
    | '[' (pattern (',' pattern)*)? ']'
    | UIDENT '(' (pattern (',' pattern)*)? ')'
    | LIDENT ('@' pattern)?;

caseExpression : 'case' expression 'of' caseBranches 'esac';
caseBranches : caseBranch ('|' caseBranch)*;
caseBranch : pattern '->' scopeExpression;

// Lexer

fragment STRING_CHAR : ~('"') | '"' '"';

UIDENT : [A-Z][a-zA-Z_0-9]*;
LIDENT : [a-z][a-zA-Z_0-9]*;
DECIMAL : [0-9]+;
STRING : '"' STRING_CHAR* '"';
CHAR : '\'' (~('\'') | '\'\'' | '\n' | '\t') '\'';

PLUS: '+';
MINUS: '-';
MUL: '*';
DIV: '/';
MOD: '%';
ASSN: ':=';
NOT: '!';
AND: '&&';
OR: '!!';
LT: '<';
GT: '>';
EQ: '==';
NE: '!=';
LE: '<=';
GE: '>=';

WS : [ \t\r\n]+ -> skip;
COMMENT : '(*' .*? '*)' -> skip;
LINE_COMMENT : '--' ~[\r\n]* -> skip;
