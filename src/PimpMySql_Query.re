/* Private */
module Sql = SqlCommon.Make_sql(MySql2);

let debug = Debug.make("bs-pimp-my-sql", "PimpMySql_Query");

let jsonToString = [%raw "(x) => JSON.stringify(x)"];

let log = (name, sql, params) => {
  debug("====================");
  debug(name);
  debug(sql);
  debug(jsonToString @@ params);
  debug("====================");
};

/* Public */
/* @TODO - make getByIdList batch large lists appropriately. */
/* @TODO - there is a bug with mysql2, getByIdList will not work until fixed*/
let getOneById = (baseQuery, table, decoder, id, conn) => {
  let sql =
    SqlComposer.Select.(
      baseQuery |> where({j|AND $table.`id` = ?|j}) |> to_sql
    );
  let params =
    Json.Encode.([|int(id)|] |> jsonArray) |> PimpMySql_Params.positional;
  log("getOneById", sql, params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       PimpMySql_Decode.oneRow(decoder, result) |> Js.Promise.resolve
     );
};

let getByIdList = (baseQuery, table, decoder, idList, conn) => {
  let sql =
    SqlComposer.Select.(
      baseQuery |> where({j| AND $table.`id` IN (?) |j}) |> to_sql
    );
  let params =
    Json.Encode.(idList |> list(int)) |> PimpMySql_Params.positional;
  log("getByIdList", sql, params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       PimpMySql_Decode.rows(decoder, result) |> Js.Promise.resolve
     );
};

let getOneBy = (baseQuery, decoder, params, conn) => {
  let sql = SqlComposer.Select.to_sql(baseQuery);
  let params = PimpMySql_Params.positional(params);
  log("getOneBy", sql, params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       PimpMySql_Decode.oneRow(decoder, result) |> Js.Promise.resolve
     );
};

let get = (baseQuery, decoder, params, conn) => {
  let sql = SqlComposer.Select.to_sql(baseQuery);
  let params = PimpMySql_Params.positional(params);
  log("get", sql, params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       PimpMySql_Decode.rows(decoder, result) |> Js.Promise.resolve
     );
};

let getWhere = (baseQuery, userQuery, decoder, params, conn) => {
  let sql =
    SqlComposer.Select.(
      {...baseQuery, where: List.concat([userQuery, baseQuery.where])}
      |> to_sql
    );
  let params = PimpMySql_Params.positional(params);
  log("getWhere", sql, params);
  Sql.Promise.query(conn, ~sql, ~params?, ())
  |> Js.Promise.then_(result =>
       PimpMySql_Decode.rows(decoder, result) |> Js.Promise.resolve
     );
};

let insertOne = (baseQuery, table, decoder, encoder, record, conn) => {
  let sql = {j|INSERT INTO $table SET ?|j};
  let params =
    [|record|] |> Json.Encode.array(encoder) |> PimpMySql_Params.positional;
  log("insertOne", sql, params);
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

let updateOneById = (baseQuery, table, decoder, encoder, record, id, conn) => {
  let sql = {j|UPDATE $table SET ? WHERE $table.`id` = ?|j};
  let params =
    Json.Encode.(
      [|encoder @@ record, int @@ id|]
      |> jsonArray
      |> PimpMySql_Params.positional
    );
  log("updateOneById", sql, params);
  getOneById(baseQuery, table, decoder, id, conn)
  |> Js.Promise.then_(res =>
       switch (res) {
       | Some(_) =>
         Sql.Promise.mutate(conn, ~sql, ~params?, ())
         |> Js.Promise.then_((_) =>
              getOneById(baseQuery, table, decoder, id, conn)
              |> Js.Promise.then_(res =>
                   Js.Promise.resolve(Result.pure(res))
                 )
            )
       | None =>
         PimpMySql_Error.NotFound("ERROR: updateOneById failed")
         |> (x => Result.error(x))
         |> Js.Promise.resolve
       }
     );
};

let archiveCompoundOneById = (baseQuery, table, decoder, id, conn) => {
  let sql = {j|
    UPDATE $table
    SET $table.`deleted` = 1, $table.`deleted_timestamp` = UNIX_TIMESTAMP()
    WHERE $table.`id` = ?
  |j};
  let params =
    Json.Encode.([|int @@ id|] |> jsonArray) |> PimpMySql_Params.positional;
  log("archiveCompoundOneById", sql, params);
  getOneById(baseQuery, table, decoder, id, conn)
  |> Js.Promise.then_(res =>
       switch (res) {
       | Some(_) =>
         Sql.Promise.mutate(conn, ~sql, ~params?, ())
         |> Js.Promise.then_((_) =>
              getOneById(baseQuery, table, decoder, id, conn)
              |> Js.Promise.then_(res =>
                   Js.Promise.resolve(Result.pure(res))
                 )
            )
       | None =>
         PimpMySql_Error.NotFound("ERROR: archiveCompoundOneById failed")
         |> (x => Result.error(x))
         |> Js.Promise.resolve
       }
     );
};

let deleteOneById = (baseQuery, table, decoder, id, conn) => {
  let sql = {j|
    DELETE FROM $table
    WHERE $table.`id` = ?
  |j};
  let params =
    Json.Encode.([|int @@ id|] |> jsonArray) |> PimpMySql_Params.positional;
  log("deleteById", sql, params);
  getOneById(baseQuery, table, decoder, id, conn)
  |> Js.Promise.then_(res =>
       switch (res) {
       | Some(x) =>
         Sql.Promise.mutate(conn, ~sql, ~params?, ())
         |> Js.Promise.then_((_) => Js.Promise.resolve(Result.pure(x)))
       | None =>
         PimpMySql_Error.NotFound("ERROR: deleteById failed")
         |> (x => Result.error(x))
         |> Js.Promise.resolve
       }
     );
};
