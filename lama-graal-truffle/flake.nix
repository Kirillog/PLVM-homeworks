{
  description = "Graal Truffle";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/336eda0d07dc5e2be1f923990ad9fdb6bc8e28e3";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem ( system:
    let
      pkgs = import nixpkgs { inherit system; };
    in {
    	devShell = pkgs.mkShell {
    	        name = "plvm-lama";
    	
    	        nativeBuildInputs = (with pkgs; [
    	          graalvm-ce
    	          maven
    	          antlr4_12
    	          gradle
    	        ]);
    	      };
    });
}
