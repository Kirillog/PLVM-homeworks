package ru.mkn.lama.parser;

import com.oracle.truffle.api.dsl.NodeFactory;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.source.Source;
import com.oracle.truffle.api.strings.TruffleString;
import org.antlr.v4.runtime.Token;
import ru.mkn.lama.LamaLanguage;
import ru.mkn.lama.nodes.FunctionRootNode;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.nodes.LamaRootNode;
import ru.mkn.lama.nodes.expr.LamaExpressionListNode;
import ru.mkn.lama.nodes.expr.LamaIntLiteralNode;
import ru.mkn.lama.nodes.expr.LamaStringLiteralNode;
import ru.mkn.lama.nodes.expr.binary.*;
import ru.mkn.lama.nodes.function.*;
import ru.mkn.lama.nodes.vars.*;

import ru.mkn.lama.parser.LamaLanguageLexer;
import ru.mkn.lama.runtime.LamaFunctionObject;

import java.util.*;
import java.util.function.Function;
import java.util.stream.IntStream;


public class LamaNodeFactory {
    private final LamaLanguage language;
    private final Source source;

    /**
     * Local variable names that are visible in the current block. Variables are not visible outside
     * of their defining block, to prevent the usage of undefined variables. Because of that, we can
     * decide during parsing if a name references a local variable or is a function name.
     */
    static class LexicalScope {
        protected final LexicalScope outer;
        protected final Map<TruffleString, Integer> locals;

        public final List<LamaNode> assns;

        LexicalScope(LexicalScope outer) {
            this.outer = outer;
            this.locals = new HashMap<>();
            this.assns = new ArrayList<>();
        }

        public Integer find(TruffleString name) {
            Integer result = locals.get(name);
            if (result != null) {
                return result;
            } else if (outer != null) {
                return outer.find(name);
            } else {
                return null;
            }
        }
    }

    private LexicalScope lexicalScope;
    private FrameDescriptor.Builder frameBuilder;
    private HashMap<TruffleString, Integer> frameArguments;
    private LamaRootNode rootNode;

    private final Map<TruffleString, LamaFunctionObject> builtins;

    public LamaNodeFactory(LamaLanguage language, Source source) {
        this.language = language;
        this.source = source;
        this.frameBuilder = FrameDescriptor.newBuilder();
        this.builtins = new HashMap<>();
        defineBuiltInFunction("write", LamaWriteFunctionBodyNodeFactory.getInstance());
        defineBuiltInFunction("read", LamaReadFunctionBodyNodeFactory.getInstance());
    }

    private void defineBuiltInFunction(String name, NodeFactory<? extends LamaBuiltinFunctionBodyNode> nodeFactory) {
        LamaReadArgumentNode[] functionArguments = IntStream.range(0, nodeFactory.getExecutionSignature().size())
                .mapToObj(LamaReadArgumentNode::new)
                .toArray(LamaReadArgumentNode[]::new);
        var builtInFuncRootNode = new FunctionRootNode(language, nodeFactory.createNode((Object) functionArguments));
        var functionObject = new LamaFunctionObject(builtInFuncRootNode.getCallTarget());
        this.builtins.put(TruffleString.fromJavaStringUncached(name, TruffleString.Encoding.UTF_8), functionObject);
    }

    public LamaRootNode getRootNode() {
        return rootNode;
    }

    public void setMainBody(LamaNode mainBody) {
        FrameDescriptor frameDescriptor = frameBuilder.build();
        rootNode = new LamaRootNode(language, frameDescriptor, mainBody);
    }

    public void startScope() {
        lexicalScope = new LexicalScope(lexicalScope);
    }

    public LamaExpressionListNode finishScope(LamaExpressionListNode exp) {
        if (exp == null) {
            lexicalScope = lexicalScope.outer;
            return new LamaExpressionListNode(Collections.emptyList());
        }
        lexicalScope.assns.add(exp);
        final LamaExpressionListNode list = new LamaExpressionListNode(lexicalScope.assns);
        lexicalScope = lexicalScope.outer;
        return list;
    }

    public void addVariableDefinition(Token ident, LamaNode valueNode) {
        TruffleString name = asTruffleString(ident, false);
        var frameSlot = frameBuilder.addSlot(FrameSlotKind.Illegal, name, null);
        lexicalScope.locals.put(name, frameSlot);
        if (valueNode != null) {
            var assignmentNode = LamaWriteLocalVariableNodeGen.create(valueNode, frameSlot);
            lexicalScope.assns.add(assignmentNode);
        }
    }

    public LamaExpressionListNode createExpression(List<LamaNode> exprs) {
        return new LamaExpressionListNode(exprs);
    }

    public LamaNode createBinaryExpression(Token op, LamaNode left, LamaNode right) {
        return switch (op.getType()) {
            case LamaLanguageLexer.PLUS -> LamaAddNodeGen.create(left, right);
            case LamaLanguageLexer.MINUS -> LamaSubNodeGen.create(left, right);
            case LamaLanguageLexer.MUL -> LamaMulNodeGen.create(left, right);
            case LamaLanguageLexer.DIV -> LamaDivNodeGen.create(left, right);
            case LamaLanguageLexer.MOD -> LamaModNodeGen.create(left, right);
            case LamaLanguageLexer.EQ -> LamaEqNodeGen.create(left, right);
            case LamaLanguageLexer.NE -> LamaNeNodeGen.create(left, right);
            case LamaLanguageLexer.GE -> LamaGeNodeGen.create(left, right);
            case LamaLanguageLexer.GT -> LamaGtNodeGen.create(left, right);
            case LamaLanguageLexer.LE -> LamaLeNodeGen.create(left, right);
            case LamaLanguageLexer.LT -> LamaLtNodeGen.create(left, right);
            default -> throw new UnsupportedOperationException("Create binary expression");
        };
    }

    public LamaNode createAssignment(LamaNode ref, LamaNode value) {
        if (ref instanceof LamaReadLocalVariableNode readLocal) {
            return LamaWriteLocalVariableNodeGen.create(value, readLocal.getSlot());
        } else {
            throw new UnsupportedOperationException("Create assignment");
        }
    }

    public LamaNode createIntLiteral(Token constant) {
        return new LamaIntLiteralNode(Integer.parseInt(constant.getText()));
    }

    public LamaNode createStringLiteral(Token constant) {
        return new LamaStringLiteralNode(asTruffleString(constant, true));
    }

    private TruffleString asTruffleString(Token literalToken, boolean removeQuotes) {
        int fromIndex = 0;
        int length = literalToken.getStopIndex() - literalToken.getStartIndex() + 1;
        if (removeQuotes) {
            /* Remove the trailing and ending " */
            assert literalToken.getText().length() >= 2 && literalToken.getText().startsWith("\"") && literalToken.getText().endsWith("\"");
            fromIndex = 1;
            length -= 2;
        }
        return TruffleString.fromJavaStringUncached(literalToken.getText(), fromIndex, length, TruffleString.Encoding.UTF_8, true);
    }

    public LamaNode createIdentifier(Token ident) {
        TruffleString name = asTruffleString(ident, false);
        final Integer frame = lexicalScope.find(name);
        final LamaNode result;
        if (frame != null) {
            result = LamaReadLocalVariableNodeGen.create(frame);
        } else if (frameArguments != null && frameArguments.containsKey(name)) {
            result = new LamaReadArgumentNode(frameArguments.get(name));
        } else if (builtins.containsKey(name)){
            result = new LamaFunctionWrapper(builtins.get(name));
        } else {
            throw new UnsupportedOperationException("Create identifier");
        }
        return result;
    }

    public LamaNode createCallExpression(String name, LamaNode function, List<LamaNode> parameters) {
        return new LamaFunctionCallNode(name, function, parameters);
    }
}
