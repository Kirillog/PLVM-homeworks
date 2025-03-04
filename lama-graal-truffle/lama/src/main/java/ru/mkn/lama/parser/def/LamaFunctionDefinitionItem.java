package ru.mkn.lama.parser.def;

import org.antlr.v4.runtime.Token;
import ru.mkn.lama.nodes.LamaNode;

import java.util.List;

public record LamaFunctionDefinitionItem(Token ident, List<Token> args, LamaNode body) implements LamaDefinitionItem {
}
