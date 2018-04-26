open Jest;

type animal = {type_: string};

external animalToJson : animal => Js.Json.t = "%identity";

describe("Decode", () => {
  let decoder = json => json;
  let cat = animalToJson({type_: "cat"});
  test("oneRow (1 result)", () =>
    switch (Decode.oneRow(decoder) @@ ([|cat|], None)) {
    | Some(_) => pass
    | None => fail("not the expected result")
    }
  );
  test("oneRow (no results)", () =>
    switch (Decode.oneRow(decoder) @@ ([||], None)) {
    | None => pass
    | Some(_) => fail("not the expected result")
    }
  );
  test("oneRow (exception)", () =>
    switch (Decode.oneRow(decoder) @@ ([|cat, cat|], None)) {
    | exception _ => pass
    | _ => fail("not the expected result")
    }
  );
  test("rows (2 results)", () =>
    switch (Decode.rows(decoder) @@ ([|cat, cat|], None)) {
    | [|_, _|] => pass
    | _ => fail("not the expected result")
    }
  );
  test("rows (no results)", () =>
    switch (Decode.rows(decoder) @@ ([||], None)) {
    | [||] => pass
    | _ => fail("not the expected result")
    }
  );
});
