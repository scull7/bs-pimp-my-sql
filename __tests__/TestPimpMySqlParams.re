open Jest;

type animal = {type_: string};

external animalToJson : animal => Js.Json.t = "%identity";

describe("PimpMySql_Params", () => {
  let json = animalToJson({type_: "blobfish"});
  test("named", () =>
    switch (PimpMySql_Params.named @@ json) {
    | Some(`Named(_)) => pass
    | _ => fail("not the expected return type")
    }
  );
  test("positional", () =>
    switch (PimpMySql_Params.positional @@ [|json|]) {
    | Some(`Positional(_)) => pass
    | _ => fail("not the expected return type")
    }
  );
});
