package ru.mkn.lama.parser.def;

import org.antlr.v4.runtime.Token;
import ru.mkn.lama.nodes.LamaNode;

public record LamaVariableDefinitionItem(Token name, LamaNode value) implements LamaDefinitionItem {

}
