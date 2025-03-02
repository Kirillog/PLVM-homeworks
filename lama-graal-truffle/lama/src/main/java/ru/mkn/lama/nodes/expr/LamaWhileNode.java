package ru.mkn.lama.nodes.expr;

import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.LoopNode;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.RepeatingNode;
import ru.mkn.lama.LamaTypesGen;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaNull;

@NodeInfo(shortName = "while", description = "The node implementing a while loop")
public final class LamaWhileNode extends LamaNode {

    @Node.Child
    private LoopNode loopNode;

    public LamaWhileNode(LamaNode conditionNode, LamaNode bodyNode) {
        this.loopNode = Truffle.getRuntime().createLoopNode(new WhileRepeatingNode(conditionNode, bodyNode));
    }

    @Override
    public Object executeGeneric(VirtualFrame frame) {
        loopNode.execute(frame);
        return LamaNull.INSTANCE;
    }

    private static final class WhileRepeatingNode extends Node implements RepeatingNode {
        @Child
        private LamaNode cond;
        @Child
        private LamaNode body;

        private WhileRepeatingNode(LamaNode cond, LamaNode body) {
            this.cond = cond;
            this.body = body;
        }

        @Override
        public boolean executeRepeating(VirtualFrame frame) {
            if (!LamaTypesGen.asImplicitBoolean(cond.executeGeneric(frame))) {
                return false;
            }
            body.executeGeneric(frame);
            return true;
        }
    }
}
