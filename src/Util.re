module Sql = SqlCommon.Make_sql(MySql2);

module Decode = {
  let date = (json: Js.Json.t) : Js.Date.t =>
    switch (Js.typeof(json)) {
    | "object" => (Obj.magic(json): Js.Date.t)
    | "string" => Js.Date.fromString(Obj.magic(json): string)
    | x => failwith({j|Invalid Date: $x|j})
    };
  let timestamp = json => date(json) |> Js.Date.getTime |> int_of_float;
  let oneRow = (decoder, (rows, _)) =>
    switch (Belt_Array.length(rows)) {
    | 1 => Some(decoder(rows[0]))
    | 0 => None
    | _ => failwith("unexpected_result_count")
    };
  let rows = (decoder, (rows, _)) => Belt_Array.map(rows, decoder);
};

module Params = {
  let named = json => Some(`Named(json));
  let positional = json => Some(`Positional(json));
};
