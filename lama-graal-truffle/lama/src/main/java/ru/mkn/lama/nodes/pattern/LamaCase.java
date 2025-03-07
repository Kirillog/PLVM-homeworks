package ru.mkn.lama.nodes.pattern;

import ru.mkn.lama.nodes.LamaNode;

public record LamaCase(LamaPattern pattern, LamaNode defs, LamaNode body) {
}
