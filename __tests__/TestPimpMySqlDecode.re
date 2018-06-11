open Jest;

type animal = {type_: string};

let animalToJson = ({type_}) =>
  Json.Encode.(object_([("type_", string @@ type_)]));

describe("PimpMySql_Decode", () => {
  let decoder = json => Json.Decode.(field("type_", string, json));
  let cat = animalToJson({type_: "cat"});
  Util.testFutureResult("oneRow (1 resutl)", () =>
    Future.value(([|cat|], None))
    |. Future.flatMap(PimpMySql_Decode.oneRow(decoder, _))
    |. Future.mapOk(
         fun
         | Some(str) => Expect.expect(str) |> Expect.toBe("cat")
         | None => fail("unexpected empty result"),
       )
  );
  Util.testFutureResult("oneRow (no results)", () =>
    Future.value(([||], None))
    |. Future.flatMap(PimpMySql_Decode.oneRow(decoder, _))
    |. Future.mapOk(
         fun
         | Some(x) => fail({j|Unexpected result: $x|j})
         | None => pass,
       )
  );
  Util.testFutureResult("oneRow (exception)", () =>
    Future.value(([|cat, cat|], None))
    |. Future.flatMap(PimpMySql_Decode.oneRow(decoder, _))
    |. Future.mapOk(x => x |. Js.String.make |. fail)
    |. Future.flatMapError(
         fun
         | PimpMySql_Error.UnexpectedRowCount(str) =>
           Expect.expect(str)
           |> Expect.toBe("Expected: 1, received: 2")
           |> (x => x |. Belt.Result.Ok |. Future.value)
         | x => x |. Js.String.make |. fail |. Belt.Result.Ok |. Future.value,
       )
  );
  Util.testFutureResult("rows (2 results)", () =>
    Future.value(([|cat, cat|], None))
    |. Future.flatMap(PimpMySql_Decode.rows(decoder, _))
    |. Future.mapOk(x =>
         Expect.expect(x) |> Expect.toEqual([|"cat", "cat"|])
       )
  );
  Util.testFutureResult("rows (no results)", () =>
    Future.value(([||], None))
    |. Future.flatMap(PimpMySql_Decode.rows(decoder, _))
    |. Future.mapOk(x => Expect.expect(x) |> Expect.toEqual([||]))
  );

  Util.testFutureResult("rows (bad decoder exception)", () => {
    let decoder = json => Json.Decode.(field("type_", int, json));
    Future.value(([|cat, cat|], None))
    |. Future.flatMap(PimpMySql_Decode.rows(decoder, _))
    |. Future.mapOk(x => x |. Js.String.make |. fail)
    |. Future.flatMapError(
         fun
         | PimpMySql_Error.JsonDecodeError(_) =>
           pass |. Belt.Result.Ok |. Future.value
         | x => x |. Js.String.make |. fail |. Belt.Result.Ok |. Future.value,
       );
  });

  Util.testFutureResult("rows (unknown exception)", () => {
    let decoder = _ => raise(PimpMySql_Error.QueryFailure("boom!"));
    Future.value(([|cat, cat|], None))
    |. Future.flatMap(PimpMySql_Decode.rows(decoder, _))
    |. Future.mapOk(x => x |. Js.String.make |. fail)
    |. Future.flatMapError(
         fun
         | PimpMySql_Error.JsonDecodeUnknownError(str) =>
           str
           |> Expect.expect
           |> Expect.toMatch("boom!")
           |. Belt.Result.Ok
           |. Future.value
         | x => x |. Js.String.make |. fail |. Belt.Result.Ok |. Future.value,
       );
  });
});
