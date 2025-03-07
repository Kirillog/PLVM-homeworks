package ru.mkn.lama.nodes.pattern;

import com.oracle.truffle.api.frame.VirtualFrame;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.LamaNull;
import ru.mkn.lama.runtime.LamaSExpObject;

public class LamaCaseExpression extends LamaNode {

    @Child
    LamaNode scrutinee;
    LamaCase[] cases;

    public LamaCaseExpression(LamaNode scrutinee, LamaCase[] cases) {
        this.scrutinee = scrutinee;
        this.cases = cases;
    }

    boolean matches(Object object, LamaPattern p) {
        if (p instanceof LamaPattern.Decimal d && object instanceof Integer i) {
            return d.number().compareTo(i) == 0;
        } else if (p instanceof LamaPattern.Wildcard) {
            return true;
        } else if (p instanceof LamaPattern.SExpPattern ps && object instanceof LamaSExpObject s) {
            if (!(ps.name().equals(s.getName()) && ps.elems().length == s.length())) {
                return false;
            }
            for (int i = 0; i < s.length(); ++i) {
                if (!matches(s.get(i), ps.elems()[i])) {
                    return false;
                }
            }
            return true;
        } else if (p instanceof LamaPattern.NamedPattern pn) {
            if (pn.pattern() == null) {
                return true;
            }
            return matches(object, pn.pattern());
        } else {
            return false;
        }
    }

    @Override
    public Object executeGeneric(VirtualFrame frame) {
        var scr = scrutinee.executeGeneric(frame);
        for (LamaCase c : cases) {
            if (matches(scr, c.pattern())) {
                c.defs().executeGeneric(frame);
                return c.body().executeGeneric(frame);
            }
        }
        return LamaNull.INSTANCE;
    }
}
