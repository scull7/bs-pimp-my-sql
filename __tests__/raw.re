open Jest;

open Util.Operators;

let connect = () =>
  MySql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

type result = {search: string};

type add_result = {result: int};

describe("Test Basic Query Interpolation", () => {
  let db = connect();
  let decoder = json => {
    search: json |> Json.Decode.field("search", Json.Decode.string)
  };
  let extractRows = results =>
    switch results {
    | PimpMySql.Promise.Mutation(_) => failwith("unexpected_mutation_result")
    | PimpMySql.Promise.Select(s) => s.rows
    };
  let add_decoder = json => {
    result: json |> Json.Decode.field("result", Json.Decode.int)
  };
  testPromise("Simple string interpolation query", () =>
    PimpMySql.raw(~db, ~sql="SELECT ? AS search", ~params=[|"%schema"|], ())
    >>= extractRows
    >>= Js.Array.map(decoder)
    >>= Js.Array.map(x => x.search)
    >>= Expect.expect
    >>= Expect.toBeSupersetOf([|"%schema"|])
  );
  testPromise("Interpolation of two positional parameters", () =>
    PimpMySql.raw(~db, ~sql="SELECT 1 + ? + ? AS result", ~params=[|5, 6|], ())
    >>= extractRows
    >>= Js.Array.map(add_decoder)
    >>= Js.Array.map(x => x.result)
    >>= Expect.expect
    >>= Expect.toBeSupersetOf([|12|])
  );
});
