package ru.mkn.lama.nodes.expr;

import com.oracle.truffle.api.dsl.NodeChild;
import ru.mkn.lama.nodes.LamaNode;


@NodeChild("leftNode")
@NodeChild("rightNode")
public abstract class LamaBinaryExpression extends LamaNode {
}
