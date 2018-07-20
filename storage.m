(* ::Package:: *)

BeginPackage["Storage`"]
StoreSymbol::usage = "Stores something in the table";
LoadSymbol::usage = "Loads something from the table";
Begin["`Private`"]
SymbolTable = <||>;
StoreSymbol[symbol_, key_: 0] := Block[{k},
   k = If[key != 0, ToString[key], ToString[UnixTime[]] <> ToString[RandomReal[]]];
   AppendTo[SymbolTable, <|k -> symbol|>];
   k
];
LoadSymbol[key_] := Block[{success, value},
   success = KeyExistsQ[SymbolTable, key];
   If[success, SymbolTable[key], Null]
];
SymbolExists[key_] := KeyExistsQ[SymbolTable, key];
RemoveSymbol[key_] := (SymbolTable = Delete[SymbolTable, key]);
End[] 
EndPackage[];
