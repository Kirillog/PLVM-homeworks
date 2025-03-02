package ru.mkn.lama;

import org.graalvm.polyglot.Context;
import org.graalvm.polyglot.PolyglotException;
import org.graalvm.polyglot.Source;
import org.graalvm.polyglot.Value;

import java.io.*;

public final class LamaMain {

    private static final String Lama = "lama";

    /**
     * The main entry point.
     */
    public static void main(String[] args) throws IOException {
        Source source;
        if (args.length != 3) {
            System.err.println("Incorrect number of arguments");
            System.exit(1);
        }

        File lamaFile = new File(args[0]);
        InputStream input = new FileInputStream(new File(args[1]));
        PrintStream output = new PrintStream(new FileOutputStream(new File(args[2])));

        source = Source.newBuilder(Lama, lamaFile).interactive(false).build();

        System.exit(executeSource(source, input, output));
    }

    private static int executeSource(Source source, InputStream in, PrintStream out) {
        Context context;
        PrintStream err = System.err;
        try {
            context = Context.newBuilder(Lama).in(in).out(out).allowAllAccess(true).build();
        } catch (IllegalArgumentException e) {
            err.println(e.getMessage());
            return 1;
        }

        try {
            Value result = context.eval(source);
            if (!result.isNull()) {
                out.println(result.toString());
            }
            return 0;
        } catch (PolyglotException ex) {
            if (ex.isInternalError()) {
                // for internal errors we print the full stack trace
                ex.printStackTrace();
            } else {
                err.println(ex.getMessage());
            }
            return 1;
        } finally {
            context.close();
        }
    }

}
