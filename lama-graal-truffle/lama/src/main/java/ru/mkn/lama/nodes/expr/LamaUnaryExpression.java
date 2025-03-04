package ru.mkn.lama.nodes.expr;

import com.oracle.truffle.api.dsl.NodeChild;
import ru.mkn.lama.nodes.LamaNode;

@NodeChild("value")
public abstract class LamaUnaryExpression extends LamaNode {
}
