/* Private */
module Sql = SqlCommon.Make_sql(MySql2);

/* Public */
/* @TODO - make getByIdList batch large lists appropriately. */
/* @TODO - there is a bug with mysql2, getByIdList will not work until fixed*/
let getById = (baseQuery, table, decoder, id, conn) => {
  let sql =
    SqlComposer.Select.(
      baseQuery |> where({j|AND $table.`id` = ?|j}) |> to_sql
    );
  let params = Some(`Positional(Json.Encode.([|int(id)|] |> jsonArray)));
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       Decode.oneRow(decoder, result) |> Js.Promise.resolve
     );
};

let getByIdList = (baseQuery, table, decoder, idList, conn) => {
  let sql =
    SqlComposer.Select.(
      baseQuery |> where({j| AND $table.`id` IN (?) |j}) |> to_sql
    );
  let params = Json.Encode.(idList |> list(int)) |> Params.positional;
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       Decode.rows(decoder, result) |> Js.Promise.resolve
     );
};

let getOneBy = (decoder, sql, params, conn) => {
  let params = Params.positional(params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       Decode.oneRow(decoder, result) |> Js.Promise.resolve
     );
};

let get = (decoder, sql, params, conn) => {
  let params = Params.positional(params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       Decode.rows(decoder, result) |> Js.Promise.resolve
     );
};

let insert = (baseQuery, table, decoder, encoder, record, conn) => {
  let params = [|record|] |> Json.Encode.array(encoder) |> Params.positional;
  let sql = {j|INSERT INTO $table SET ?|j};
  Sql.Promise.mutate(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(((_, id)) =>
       getById(baseQuery, table, decoder, id, conn)
     );
};

let insertBatch =
    (~name, ~table, ~encoder, ~loader, ~error, ~columns, ~rows, conn) =>
  switch (rows) {
  | [||] => `Ok([||]) |> Js.Promise.resolve
  | _ =>
    Sql.Promise.mutate_batch(
      conn,
      ~batch_size=?None,
      ~table,
      ~columns=Belt_Array.map(columns, Json.Encode.string),
      ~rows=Belt_Array.map(rows, encoder),
    )
    |> Js.Promise.then_((_) => loader(rows))
    |> Js.Promise.then_(result => `Ok(result) |> Js.Promise.resolve)
    |> Js.Promise.catch(e =>
         {j|ERROR: $name - $e|j}
         |> error
         |> (x => `Error(x))
         |> Js.Promise.resolve
       )
  };
