package ru.mkn.lama.parser;

import com.oracle.truffle.api.dsl.NodeFactory;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.nodes.NodeUtil;
import com.oracle.truffle.api.source.Source;
import com.oracle.truffle.api.strings.TruffleString;
import org.antlr.v4.runtime.Token;
import ru.mkn.lama.LamaLanguage;
import ru.mkn.lama.nodes.FunctionRootNode;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.nodes.LamaRootNode;
import ru.mkn.lama.nodes.expr.*;
import ru.mkn.lama.nodes.expr.binary.*;
import ru.mkn.lama.nodes.expr.literal.LamaIntLiteralNode;
import ru.mkn.lama.nodes.expr.literal.LamaStringLiteralNode;
import ru.mkn.lama.nodes.expr.unary.LamaNegateNodeGen;
import ru.mkn.lama.nodes.function.*;
import ru.mkn.lama.nodes.vars.*;

import ru.mkn.lama.parser.def.LamaDefinitionItem;
import ru.mkn.lama.parser.def.LamaFunctionDefinitionItem;
import ru.mkn.lama.parser.def.LamaVariableDefinitionItem;
import ru.mkn.lama.runtime.LamaFunctionObject;

import java.util.*;
import java.util.stream.IntStream;


public class LamaNodeFactory {
    private final LamaLanguage language;
    private final Source source;


    interface Identifier {}

    static class LocalIdentifier implements Identifier {
        Integer frameSlot;

        LocalIdentifier(Integer frameSlot) {
            this.frameSlot = frameSlot;
        }
    }

    static class Parameter implements Identifier {
        Integer number;

        Parameter(Integer frameSlot) {
            this.number = frameSlot;
        }
    }


    static class GlobalIdentifier implements Identifier {
        TruffleString name;

        GlobalIdentifier(TruffleString name) {
            this.name = name;
        }
    }

    /**
     * Local variable names that are visible in the current block. Variables are not visible outside
     * of their defining block, to prevent the usage of undefined variables. Because of that, we can
     * decide during parsing if a name references a local variable or is a function name.
     */
    static class LexicalScope {
        protected final LexicalScope outer;
        protected final Map<TruffleString, Identifier> locals;

        public final List<LamaNode> assns;

        LexicalScope(LexicalScope outer) {
            this.outer = outer;
            this.locals = new HashMap<>();
            this.assns = new ArrayList<>();
        }

        public Identifier find(TruffleString name) {
            Identifier result = locals.get(name);
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

    static class FrameList {
        private final FrameDescriptor.Builder frameBuilder;
        private final FrameList outer;

        FrameList(FrameList outer) {
            this.frameBuilder = FrameDescriptor.newBuilder();
            this.outer = outer;
        }
    }

    private FrameList frames;
    private LamaRootNode rootNode;

    private final Map<TruffleString, LamaFunctionObject> builtins;

    public LamaNodeFactory(LamaLanguage language, Source source) {
        this.language = language;
        this.source = source;
        this.frames = new FrameList(null);
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
        builtins.put(TruffleString.fromJavaStringUncached(name, TruffleString.Encoding.UTF_8), functionObject);
    }

    public LamaRootNode getRootNode() {
        return rootNode;
    }

    public void setMainBody(LamaNode mainBody) {
        FrameDescriptor frameDescriptor = frames.frameBuilder.build();
        rootNode = new LamaRootNode(language, frameDescriptor, mainBody);
    }

    public void startScope() {
        lexicalScope = new LexicalScope(lexicalScope);
    }

    public LamaExpressionListNode finishScope(LamaExpressionListNode exp, boolean expand) {
        if (exp == null) {
            if (!expand) {
                lexicalScope = lexicalScope.outer;
            }
            return new LamaExpressionListNode(Collections.emptyList());
        }
        lexicalScope.assns.add(exp);
        final LamaExpressionListNode list = new LamaExpressionListNode(lexicalScope.assns);
        if (!expand) {
            lexicalScope = lexicalScope.outer;
        }
        return list;
    }

    public void addVariableDefinition(Token ident, LamaNode valueNode) {
        TruffleString name = asTruffleString(ident, false);
        if (lexicalScope.outer == null) {
            lexicalScope.locals.put(name, new GlobalIdentifier(name));
            if (valueNode != null) {
                var assignmentNode = LamaWriteGlobalVariableNodeGen.create(valueNode, name);
                lexicalScope.assns.add(assignmentNode);
            }
        } else {
            var frameSlot = frames.frameBuilder.addSlot(FrameSlotKind.Illegal, name, null);
            lexicalScope.locals.put(name, new LocalIdentifier(frameSlot));
            if (valueNode != null) {
                var assignmentNode = LamaWriteLocalVariableNodeGen.create(valueNode, frameSlot);
                lexicalScope.assns.add(assignmentNode);
            }
        }
    }


    public LamaExpressionListNode createExpression(List<LamaNode> exprs) {
        return new LamaExpressionListNode(exprs);
    }

    public LamaNode createUnaryExpression(Token op, LamaNode exp) {
        return switch (op.getType()) {
            case LamaLanguageLexer.MINUS -> LamaNegateNodeGen.create(exp);
            default -> throw new UnsupportedOperationException("Create unary expression");
        };
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
            case LamaLanguageLexer.AND -> LamaAndNodeGen.create(left, right);
            case LamaLanguageLexer.OR -> LamaOrNodeGen.create(left, right);
            default -> throw new UnsupportedOperationException("Create binary expression");
        };
    }

    public LamaNode createAssignment(LamaNode ref, LamaNode value) {
        if (ref instanceof LamaReadLocalVariableNode readLocal) {
            return LamaWriteLocalVariableNodeGen.create(value, readLocal.getSlot());
        } else if (ref instanceof LamaReadGlobalVariableNode readGlobal) {
            return LamaWriteGlobalVariableNodeGen.create(value, readGlobal.getName());
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

    private static TruffleString asTruffleString(Token literalToken, boolean removeQuotes) {
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
        final Identifier scopedIdent = lexicalScope.find(name);
        final LamaNode result;
        if (scopedIdent != null) {
            if (scopedIdent instanceof GlobalIdentifier it) {
                result = new LamaReadGlobalVariableNode(name);
            } else if (scopedIdent instanceof LocalIdentifier it) {
                result = LamaReadLocalVariableNodeGen.create(it.frameSlot);
            } else if (scopedIdent instanceof Parameter it) {
                result = new LamaReadArgumentNode(it.number);
            } else {
                throw new UnsupportedOperationException("Create identifier");
            }
        } else if (builtins.containsKey(name)) {
            result = new LamaFunctionWrapper(builtins.get(name));
        } else {
            // NOTE(kmitkin): incorrect semantics, should be definition expansion but
            // it requires total rewriting of parsing to introduce definition gathering step
            // so temporary introduced to pass through tests
            result = new LamaReadGlobalVariableNode(name);
        }
        return result;
    }

    public LamaNode createCallExpression(String name, LamaNode function, List<LamaNode> parameters) {
        return new LamaFunctionCallNode(name, function, parameters);
    }

    public LamaNode createWhileExpression(LamaNode cond, LamaNode body) {
        return new LamaWhileNode(cond, body);
    }

    public LamaNode createIfExpression(LamaNode cond, LamaNode body, LamaNode elsePart) {
        return new LamaIfNode(cond, body, elsePart);
    }

    public LamaNode createDoWhileExpression(LamaNode cond, LamaNode body) {
        return new LamaExpressionListNode(body, new LamaWhileNode(cond, body));
    }

    public LamaNode createSkipExpression() {
        return new LamaSkipNode();
    }

    public LamaNode createForExpression(LamaNode before, LamaNode cond, LamaNode step, LamaNode body) {
        lexicalScope = lexicalScope.outer;
        return new LamaExpressionListNode(before, new LamaWhileNode(cond, new LamaExpressionListNode(body, step)));
    }

    public void enterFunction(List<Token> args) {
        frames = new FrameList(frames);
        startScope();
        for (int i = 0; i < args.size(); ++i) {
            lexicalScope.locals.put(asTruffleString(args.get(i), false), new Parameter(i));
        }
    }

    public void leaveFunction(Token ident, LamaNode body) {
        FrameDescriptor desc = frames.frameBuilder.build();
        frames = frames.outer;
        var root = new FunctionRootNode(language, body, desc);
        NodeUtil.printTree(System.out, root);
        var obj = new LamaFunctionObject(root.getCallTarget());
        lexicalScope = lexicalScope.outer;
        addVariableDefinition(ident, new LamaFunctionWrapper(obj));
    }
}
