{
  description = "Graal Truffle";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.11";
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
