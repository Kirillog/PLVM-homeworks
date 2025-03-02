package ru.mkn.lama.runtime;
import com.oracle.truffle.api.CallTarget;
import com.oracle.truffle.api.interop.TruffleObject;
import ru.mkn.lama.nodes.function.LamaFunctionDispatchNode;
import ru.mkn.lama.nodes.function.LamaFunctionDispatchNodeGen;

public final class LamaFunctionObject implements TruffleObject {
    public final CallTarget callTarget;

    private final LamaFunctionDispatchNode dispatchNode;

    public LamaFunctionObject(CallTarget callTarget) {
        this.callTarget = callTarget;
        this.dispatchNode = LamaFunctionDispatchNodeGen.create();
    }

    Object execute(Object[] arguments) {
        return this.dispatchNode.executeDispatch(this, arguments);
    }
}
