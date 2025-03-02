package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.dsl.GenerateNodeFactory;
import com.oracle.truffle.api.dsl.NodeChild;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.nodes.vars.LamaReadArgumentNode;

@NodeChild(value = "arguments", type = LamaReadArgumentNode[].class)
@GenerateNodeFactory
public abstract class LamaBuiltinFunctionBodyNode extends LamaNode {
}
