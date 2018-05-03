/* Private */
module Sql = SqlCommon.Make_sql(MySql2);

let oneRow = (decoder, (rows, _)) =>
  switch (Belt_Array.length(rows)) {
  | 1 => Some(decoder(rows[0]))
  | 0 => None
  | _ => failwith("unexpected_result_count")
  };

let rows = (decoder, (rows, _)) => Belt_Array.map(rows, decoder);

/* Public */
/* @TODO - make getByIdList batch large lists appropriately. */
/* @TODO - there is a bug with mysql2, getByIdList will not work until fixed*/
let getOneById = (baseQuery, table, decoder, id, conn) => {
  let sql =
    SqlComposer.Select.(
      baseQuery |> where({j|AND $table.`id` = ?|j}) |> to_sql
    );
  let params = Some(`Positional(Json.Encode.([|int(id)|] |> jsonArray)));
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result => oneRow(decoder, result) |> Js.Promise.resolve);
};

let getByIdList = (baseQuery, table, decoder, idList, conn) => {
  let sql =
    SqlComposer.Select.(
      baseQuery |> where({j| AND $table.`id` IN (?) |j}) |> to_sql
    );
  let params =
    Json.Encode.(idList |> list(int)) |> PimpMySql_Params.positional;
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result => rows(decoder, result) |> Js.Promise.resolve);
};

let getOneBy = (decoder, sql, params, conn) => {
  let params = PimpMySql_Params.positional(params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result => oneRow(decoder, result) |> Js.Promise.resolve);
};

let get = (decoder, sql, params, conn) => {
  let params = PimpMySql_Params.positional(params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result => rows(decoder, result) |> Js.Promise.resolve);
};

let insert = (baseQuery, table, decoder, encoder, record, conn) => {
  let params =
    [|record|] |> Json.Encode.array(encoder) |> PimpMySql_Params.positional;
  let sql = {j|INSERT INTO $table SET ?|j};
  Sql.Promise.mutate(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(((_, id)) =>
       getOneById(baseQuery, table, decoder, id, conn)
     );
};

let insertBatch =
    (~name, ~table, ~encoder, ~loader, ~error, ~columns, ~rows, conn) =>
  switch (rows) {
  | [||] => Result.pure([||]) |> Js.Promise.resolve
  | _ =>
    Sql.Promise.mutate_batch(
      conn,
      ~batch_size=?None,
      ~table,
      ~columns=Belt_Array.map(columns, Json.Encode.string),
      ~rows=Belt_Array.map(rows, encoder),
    )
    |> Js.Promise.then_((_) => loader(rows))
    |> Js.Promise.then_(result => Result.pure(result) |> Js.Promise.resolve)
    |> Js.Promise.catch(e =>
         {j|ERROR: $name - $e|j}
         |> error
         |> (x => Result.error(x))
         |> Js.Promise.resolve
       )
  };

let updateById = (baseQuery, table, decoder, encoder, record, id, conn) => {
  let params =
    Json.Encode.(
      [|encoder @@ record, int @@ id|]
      |> jsonArray
      |> PimpMySql_Params.positional
    );
  let sql = {j|UPDATE $table SET ? WHERE $table.`id` = ?|j};
  Sql.Promise.mutate(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(((success, _)) =>
       if (success == 1) {
         getOneById(baseQuery, table, decoder, id, conn)
         |> Js.Promise.then_(res => Js.Promise.resolve(Result.pure(res)));
       } else {
         PimpMySql_Error.NotFound("ERROR: updateById failed")
         |> (x => Result.error(x))
         |> Js.Promise.resolve;
       }
     );
};

let archiveCompoundById = (baseQuery, table, decoder, id, conn) => {
  let params =
    Json.Encode.([|int @@ id|] |> jsonArray |> PimpMySql_Params.positional);
  let sql = {j|
    UPDATE $table
    SET $table.`deleted` = 1, $table.`deleted_timestamp` = NOW()
    WHERE $table.`id` = ?
  |j};
  Sql.Promise.mutate(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(((success, _)) =>
       if (success == 1) {
         getOneById(baseQuery, table, decoder, id, conn)
         |> Js.Promise.then_(res => Js.Promise.resolve(Result.pure(res)));
       } else {
         PimpMySql_Error.NotFound("ERROR: softCompoundDelete failed")
         |> (x => Result.error(x))
         |> Js.Promise.resolve;
       }
     );
};
