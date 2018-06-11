module Exn = PimpMySql_Error;

let wrap = (json, decoder) =>
  try (decoder(json) |. Belt.Result.Ok) {
  | Json.Decode.DecodeError(str) =>
    Exn.JsonDecodeError(str) |. Belt.Result.Error
  | x => Js.String.make(x) |. Exn.JsonDecodeUnknownError |. Belt.Result.Error
  };

let expectedOne = count =>
  count
  |. string_of_int
  |. (x => {j|Expected: 1, received: $x|j})
  |. PimpMySql_Error.UnexpectedRowCount;

let oneRow = (decoder, (rows, _)) =>
  (
    switch (Belt_Array.length(rows)) {
    | 1 => rows[0] |. wrap(decoder) |. Belt.Result.map(x => Some(x))
    | 0 => Belt.Result.Ok(None)
    | x => expectedOne(x) |. Belt.Result.Error
    }
  )
  |. Future.value;

let rows = (decoder, (rows, _)) =>
  (
    switch (rows) {
    | [||] => Belt.Result.Ok([||])
    | r => wrap(r, row => Belt.Array.map(row, decoder))
    }
  )
  |. Future.value;
