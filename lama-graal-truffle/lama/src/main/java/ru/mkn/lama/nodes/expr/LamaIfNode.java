package ru.mkn.lama.nodes.expr;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.Node;
import com.oracle.truffle.api.nodes.NodeInfo;
import com.oracle.truffle.api.nodes.UnexpectedResultException;
import com.oracle.truffle.api.profiles.ConditionProfile;

import ru.mkn.lama.LamaException;
import ru.mkn.lama.LamaTypesGen;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaNull;

@NodeInfo(shortName = "if", description = "The node implementing a conditional statement")
public final class LamaIfNode extends LamaNode {

    @Node.Child
    private LamaNode conditionNode;

    @Node.Child
    private LamaNode thenPartNode;

    @Node.Child
    private LamaNode elsePartNode;

    private final ConditionProfile condition = ConditionProfile.createCountingProfile();

    public LamaIfNode(LamaNode conditionNode, LamaNode thenPartNode, LamaNode elsePartNode) {
        this.conditionNode = conditionNode;
        this.thenPartNode = thenPartNode;
        this.elsePartNode = elsePartNode;
    }

    @Override
    public Object executeGeneric(VirtualFrame frame) {
        if (condition.profile(evaluateCondition(frame))) {
            return thenPartNode.executeGeneric(frame);
        } else {
            return elsePartNode == null ? LamaNull.INSTANCE : elsePartNode.executeGeneric(frame);
        }
    }

    private boolean evaluateCondition(VirtualFrame frame) {
        return LamaTypesGen.asImplicitBoolean(conditionNode.executeGeneric(frame));
    }
}

