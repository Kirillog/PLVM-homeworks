package ru.mkn.lama.nodes.pattern;

import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.ExplodeLoop;
import ru.mkn.lama.nodes.LamaNode;
import ru.mkn.lama.runtime.*;

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
        } else if (p instanceof LamaPattern.ArrayPattern ap && object instanceof LamaArrayObject a) {
            if (!(ap.elems().length == a.length())) {
                return false;
            }
            for (int i = 0; i < a.length(); ++i) {
                if (!matches(a.get(i), ap.elems()[i])) {
                    return false;
                }
            }
            return true;
        } else if (p instanceof LamaPattern.StringPattern ps && object instanceof LamaStringObject obj) {
            return ps.str().equals(obj);
        } else if (p instanceof LamaPattern.NamedPattern pn) {
            if (pn.pattern() == null) {
                return true;
            }
            return matches(object, pn.pattern());
        } else if (p instanceof LamaPattern.ConsPattern cp && object instanceof LamaArrayObject a) {
            if (a.length() != 2) {
                return false;
            }
            return matches(a.get(0), cp.h()) && matches(a.get(1), cp.t());
        } else if (p instanceof LamaPattern.ListPattern && object instanceof LamaArrayObject a) {
            return a.length() == 0;
        } else if (p instanceof LamaPattern.FuncTypePattern && object instanceof LamaFunctionObject) {
            return true;
        } else if (p instanceof LamaPattern.ValTypePattern && object instanceof Integer) {
            return true;
        } else if (p instanceof LamaPattern.StrTypePattern && object instanceof LamaStringObject) {
            return true;
        } else {
            return false;
        }
    }

    @Override
    @ExplodeLoop
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
