package ru.mkn.lama.nodes.function;

import com.oracle.truffle.api.dsl.GenerateNodeFactory;
import com.oracle.truffle.api.dsl.NodeChild;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.nodes.vars.LamaReadParameterNode;

@NodeChild(value = "arguments", type = LamaReadParameterNode[].class)
@GenerateNodeFactory
public abstract class LamaBuiltinFunctionBodyNode extends LamaNode {
}
