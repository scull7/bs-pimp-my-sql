open SqlComposer;

/* Private */
module Sql = SqlCommon.Make_sql(MySql2);
module Params = PimpMySql_Params;
module Decode = PimpMySql_Decode;
module PMSError = PimpMySql_Error;

let assertArrayNotEmpty = (msg, arr) =>
  switch (arr) {
  | [||] => Belt.Result.Error(PMSError.UnexpectedEmptyArray(msg))
  | x => Belt.Result.Ok(x)
  };

let assertHasItem = (msg, maybe) =>
  switch (maybe) {
  | Some(x) => Belt.Result.Ok(x)
  | None => Belt.Result.Error(PMSError.NotFound(msg))
  };

let checkEmptyUserQuery = (msg, arr) =>
  switch (arr) {
  | [] => Belt.Result.Error(PMSError.EmptyUserQuery(msg))
  | _ => Belt.Result.Ok(arr)
  };

let debug = Debug.make("bs-pimp-my-sql", "PimpMySql_Query");

let jsonToString = [%raw "(x) => JSON.stringify(x)"];

let log = (name, sql, params) => {
  debug("====================");
  debug(name);
  debug(sql);
  debug(jsonToString @@ params);
  debug("====================");
};

let mutate = (name, sql, params, db) => {
  let errorHandler = e => Js.String.make(e) |. PMSError.MutationFailure;
  log(name, sql, params);
  Sql.Promise.mutate(db, ~sql, ~params?, ())
  |. FutureJs.fromPromise(errorHandler);
};

let query = (name, sql, params, db) => {
  let errorHandler = e => Js.String.make(e) |. PMSError.QueryFailure;
  log(name, sql, params);
  Sql.Promise.query(db, ~sql, ~params?, ())
  |. FutureJs.fromPromise(errorHandler);
};

let queryOne = (name, decoder, sql, params, db) =>
  query(name, sql, params, db) |. Future.mapOk(Decode.oneRow(decoder));

let queryMany = (name, decoder, sql, params, db) =>
  query(name, sql, params, db) |. Future.mapOk(Decode.rows(decoder));

/* Public */
/* @TODO - make getByIdList batch large lists appropriately. */
/* @TODO - there is a bug with mysql2, getByIdList will not work until fixed*/
let getOneById = (baseQuery, table, decoder, id, db) =>
  queryOne(
    "getOneById",
    decoder,
    Select.(baseQuery |. where({j|AND $table.`id` = ?|j}) |. to_sql),
    Json.Encode.([|int @@ id|]) |. Params.positional,
    db,
  );

let getByIdList = (baseQuery, table, decoder, idList, db) =>
  queryMany(
    "getByIdList",
    decoder,
    Select.(baseQuery |. where({j| AND $table.`id` IN (?) |j}) |. to_sql),
    Json.Encode.([|idList |> list(int)|]) |> Params.positional,
    db,
  );

let getOneBy = (baseQuery, decoder, params, db) =>
  queryOne(
    "getOneBy",
    decoder,
    Select.to_sql(baseQuery),
    Params.positional(params),
    db,
  );

let get = (baseQuery, decoder, params, db) =>
  queryMany(
    "get",
    decoder,
    Select.to_sql(baseQuery),
    Params.positional(params),
    db,
  );

let getWhere = (baseQuery, userQuery, decoder, params, db) =>
  queryMany(
    "getWhere",
    decoder,
    Select.(
      Belt.List.reduce(userQuery, baseQuery, (acc, x) => acc |. where(x))
      |. to_sql
    ),
    Params.positional(params),
    db,
  );

let insertOne = (baseQuery, table, decoder, encoder, record, db) => {
  let sql = {j|INSERT INTO $table SET ?|j};
  let params = [|encoder @@ record|] |. Params.positional;
  let get = ((_, id)) => getOneById(baseQuery, table, decoder, id, db);

  mutate("insertOne", sql, params, db) |. Future.flatMapOk(get);
};

let insertBatch =
    (~name: string, ~table, ~encoder, ~loader, ~error, ~columns, ~rows, db) =>
  switch (rows) {
  | [||] => Future.value(Belt.Result.Ok([||]))
  | _ =>
    Sql.Promise.mutate_batch(
      db,
      ~batch_size=?None,
      ~table,
      ~columns=Belt_Array.map(columns, Json.Encode.string),
      ~rows=Belt_Array.map(rows, encoder),
    )
    |. FutureJs.fromPromise(e => {
         let errorString = Js.String.make(e);
         error({j|ERROR: $name - $errorString |j});
       })
    |. Future.flatMapOk(_ => loader(rows))
  };

let updateOneById = (baseQuery, table, decoder, encoder, record, id, db) => {
  let sql = {j|UPDATE $table SET ? WHERE $table.`id` = ?|j};
  let params =
    Json.Encode.([|encoder @@ record, int @@ id|] |. Params.positional);
  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(_ => mutate("updateOneById", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};

let deactivateOneById = (baseQuery, table, decoder, id, db) => {
  let sql = {j|
    UPDATE $table
    SET $table.`active` = 0
    WHERE $table.`id` = ?
  |j};
  let params = Json.Encode.([|int @@ id|]) |. Params.positional;

  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(_ => mutate("deactivateOneById", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};

let archiveOneById = (baseQuery, table, decoder, id, db) => {
  let sql = {j|
    UPDATE $table
    SET $table.`deleted` = UNIX_TIMESTAMP()
    WHERE $table.`id` = ?
  |j};
  let params = Json.Encode.([|int @@ id|]) |. Params.positional;

  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(_ => mutate("archiveOneById", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};

let archiveCompoundBy = (baseQuery, userQuery, table, decoder, params, db) => {
  let where = String.concat(" ", userQuery);
  let sql = {j|
    UPDATE $table
    SET $table.`deleted` = 1, $table.`deleted_timestamp` = UNIX_TIMESTAMP()
    WHERE 1=1 $where
  |j};
  let normalizedParams = Params.positional(params);
  let errorMsg = "ERROR: archiveCompoundBy failed";

  checkEmptyUserQuery(errorMsg, userQuery)
  |. Future.value
  |. Future.flatMapOk(x => getWhere(baseQuery, x, decoder, params, db))
  |. Future.flatMapOk(x => assertArrayNotEmpty(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(x =>
       mutate("archiveCompoundBy", sql, normalizedParams, db)
       |. (_ => Belt.Result.Ok(x) |. Future.value)
     );
};

let archiveCompoundOneById = (baseQuery, table, decoder, id, db) => {
  let sql = {j|
    UPDATE $table
    SET $table.`deleted` = 1, $table.`deleted_timestamp` = UNIX_TIMESTAMP()
    WHERE $table.`id` = ?
  |j};
  let params = Json.Encode.([|int @@ id|]) |. Params.positional;
  let errorMsg = "ERROR: archiveCompoundOneById failed";

  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(x =>
       mutate("archiveCompoundOneById", sql, params, db)
       |. (_ => Belt.Result.Ok(x) |. Future.value)
     );
};

let deleteBy = (baseQuery, userQuery, table, decoder, params, db) => {
  let where = String.concat(" ", userQuery);
  let sql = {j|
    DELETE FROM $table
    WHERE 1=1 $where
  |j};
  let normalizedParams = Params.positional(params);
  let errorMsg = "ERROR: deleteBy failed";

  checkEmptyUserQuery(errorMsg, userQuery)
  |. Future.value
  |. Future.flatMapOk(x => getWhere(baseQuery, x, decoder, params, db))
  |. Future.flatMapOk(x => assertArrayNotEmpty(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(x =>
       mutate("deleteBy", sql, normalizedParams, db) |. Future.mapOk(_ => x)
     );
};

let deleteOneById = (baseQuery, table, decoder, id, db) => {
  let sql = {j|
    DELETE FROM $table
    WHERE $table.`id` = ?
  |j};
  let params = Json.Encode.([|int @@ id|]) |. Params.positional;
  let errorMsg = "ERROR: deleteOneById failed";
  log("deleteOneById", sql, params);
  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(x =>
       mutate("deleteOneById", sql, params, db) |. Future.mapOk(_ => x)
     );
};
