1. Currently, there is a problem linked with special semantics of definitions.

    All variables are available in the current scope and inner scopes, but in current scope
    they are available independent of their position, so following is correct:
```lama
fun test() {
  a := 10
}
var a;
a
```
and returns `10` as expected. It isn't linked with closures, as it may seem on the first sight:
```lama
var b = test();
fun test() { 10 }
b
```
also returns `10`.

In an implementation of the Lama compiler written on Lama binding of variables takes care of this nuance.
We gather definitions of scope expression, introduce them into the current scope and only in this environment resolve identifiers to variables,
obviously correctly processing two examples above.

So problem is that the chosen approach of parsing with embedded actions in ANTLRv4 grammar
turned out to be very inconvenient due to impossibility to control whether call or not predefined actions for nonterminal.
For correct definition expansion we should parse all definition and **then** parse functions bodies and values of variable definitions, but we cannot, due to
call neterminal parsing have predefined embedded actions that we do not want to execute.

Other possible solution is to use dynamic variable binding (i.e. during execution of Truffle AST), which obviously will be less effective but correct

TODO: be smarter and use custom visitor for parsing.

2. Custom infix operators. Obviously due to the ability of define custom infix operators in any place in a program
on the parsing stage we get incorrect AST, because we don't know right precedence and associativity of the operator while parsing.

We need to rebuild AST with correct precedence and associativity later, starting from the binary expression nonterminal.
The most simple way is also to use ANTLRv4 visitor and switch to custom parser when custom infix operator will be declared.

