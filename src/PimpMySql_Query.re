open SqlComposer;

/* Private */
module Sql = SqlCommon.Make(MySql2);
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

let checkEmptyUserQuery = (msg, base, user) => {
  let baseSql = Select.toSql(base);
  let userSql = base |. user |. Select.toSql;

  baseSql == userSql ?
    Belt.Result.Error(PMSError.EmptyUserQuery(msg)) : Belt.Result.Ok(user);
};

let debug = Debug.make("bs-pimp-my-sql", "PimpMySql:Query");

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
  Sql.Promise.mutate(~db, ~sql, ~params?)
  |. FutureJs.fromPromise(errorHandler)
  |. Future.mapOk(mutation =>
       (
         mutation |. Sql.Response.Mutation.insertId,
         mutation |. Sql.Response.Mutation.affectedRows,
       )
     );
};

let query = (name, sql, params, db) => {
  let errorHandler = e => Js.String.make(e) |. PMSError.QueryFailure;
  log(name, sql, params);
  Sql.Promise.query(~db, ~sql, ~params?)
  |. FutureJs.fromPromise(errorHandler)
  |. Future.mapOk(select =>
       (
         select |. Sql.Response.Select.rows,
         select |. Sql.Response.Select.meta,
       )
     );
};

let queryOne = (name, decoder, sql, params, db) =>
  query(name, sql, params, db) |. Future.flatMapOk(Decode.oneRow(decoder));

let queryMany = (name, decoder, sql, params, db) =>
  query(name, sql, params, db) |. Future.flatMapOk(Decode.rows(decoder));

/* Public */
/* @TODO - make getByIdList batch large lists appropriately. */
/* @TODO - there is a bug with mysql2, getByIdList will not work until fixed*/
let getOneById = (baseQuery, table, decoder, id, db) =>
  queryOne(
    "getOneById",
    decoder,
    Select.(baseQuery |. where({j|AND $table.id = ?|j}) |. toSql),
    [|Sql.Id.toJson @@ id|] |. Params.positional,
    db,
  );

/**
 * @TODO - bs-sql-common query_batch method needs to be implemented before
 *         this will work.
 */
/*
 let getByIdList = (baseQuery, table, decoder, idList, db) =>
   queryMany(
     "getByIdList",
     decoder,
     Select.(baseQuery |. where({j| AND $table.`id` IN (?) |j}) |. to_sql),
     Json.Encode.([| list(int, idList) |]) |> Params.positional,
     db,
   );
 */

let getOneBy = (baseQuery, decoder, params, db) =>
  queryOne(
    "getOneBy",
    decoder,
    Select.toSql(baseQuery),
    Params.positional(params),
    db,
  );

let get = (baseQuery, decoder, params, db) =>
  queryMany(
    "get",
    decoder,
    Select.toSql(baseQuery),
    Params.positional(params),
    db,
  );

let getWhere = (baseQuery, userQuery, decoder, params, db) =>
  queryMany(
    "getWhere",
    decoder,
    userQuery(baseQuery) |. Select.toSql,
    Params.positional(params),
    db,
  );

let insertOne = (baseQuery, table, decoder, encoder, record, db) => {
  let sql = {j|INSERT INTO $table SET ?|j};
  let params = [|encoder @@ record|] |. Params.positional;
  let get = ((id, _)) =>
    id
    |. Belt.Option.getExn
    |. (id => getOneById(baseQuery, table, decoder, id, db));

  mutate("insertOne", sql, params, db) |. Future.flatMapOk(get);
};

let insertBatch =
    (~name: string, ~table, ~encoder, ~loader, ~error, ~columns, ~rows, db) =>
  switch (rows) {
  | [||] => Future.value(Belt.Result.Ok([||]))
  | _ =>
    Sql.Promise.Batch.mutate(
      ~db,
      ~batch_size=?None,
      ~table,
      ~columns,
      ~encoder,
      ~rows,
      (),
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
    [|encoder @@ record, Sql.Id.toJson @@ id|] |. Params.positional;
  let errorMsg = "ERROR: updateOneById failed";
  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(_ => mutate("updateOneById", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};

let deactivateOneById = (baseQuery, table, decoder, id, db) => {
  let sql =
    Update.(
      make()
      |. from(table)
      |. set({j|$table.`active`|j}, "0")
      |. where({j|AND $table.`id` = ?|j})
      |. toSql
    );
  let params = [|Sql.Id.toJson @@ id|] |. Params.positional;
  let errorMsg = "ERROR: deactivateOneById failed";

  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(_ => mutate("deactivateOneById", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};

let archiveOneById = (baseQuery, table, decoder, id, db) => {
  let sql =
    Update.(
      make()
      |. from(table)
      |. set({j|$table.`deleted`|j}, "1")
      |. where({j|AND $table.`id` = ?|j})
      |. toSql
    );
  let params = [|Sql.Id.toJson @@ id|] |. Params.positional;
  let errorMsg = "ERROR: archiveOneById failed";

  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(_ => mutate("archiveOneById", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};

let archiveCompoundBy = (baseQuery, userQuery, table, decoder, params, db) => {
  let userSelect = userQuery(baseQuery);
  let sql =
    Conversion.updateFromSelect(userSelect)
    |. Update.set({j|$table.`deleted`|j}, "1")
    |. Update.set({j|$table.`deleted_timestamp`|j}, "UNIX_TIMESTAMP()")
    |. Update.toSql;
  let p = Params.positional(params);
  let errorMsg = "ERROR: archiveCompoundBy failed";

  checkEmptyUserQuery(errorMsg, baseQuery, userQuery)
  |. Future.value
  |. Future.flatMapOk(x => getWhere(baseQuery, x, decoder, params, db))
  |. Future.flatMapOk(x => assertArrayNotEmpty(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(_ => mutate("archiveCompoundBy", sql, p, db))
  |. Future.flatMapOk(_ =>
       getWhere(baseQuery, userQuery, decoder, params, db)
     );
};

let archiveCompoundOneById = (baseQuery, table, decoder, id, db) => {
  let sql =
    Update.(
      make()
      |. from(table)
      |. set({j|$table.`deleted`|j}, "1")
      |. set({j|$table.`deleted_timestamp`|j}, "UNIX_TIMESTAMP()")
      |. where({j|AND $table.`id` = ?|j})
      |. toSql
    );
  let params = [|Sql.Id.toJson @@ id|] |. Params.positional;
  let errorMsg = "ERROR: archiveCompoundOneById failed";

  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(_ => mutate("archiveCompoundOneById", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};

let deleteBy = (baseQuery, userQuery, decoder, params, db) => {
  let sql =
    Conversion.deleteFromSelect(baseQuery |> userQuery) |. Delete.toSql;
  let normalizedParams = Params.positional(params);
  let errorMsg = "ERROR: deleteBy failed";

  checkEmptyUserQuery(errorMsg, baseQuery, userQuery)
  |. Future.value
  |. Future.flatMapOk(x => getWhere(baseQuery, x, decoder, params, db))
  |. Future.flatMapOk(x => assertArrayNotEmpty(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(x =>
       mutate("deleteBy", sql, normalizedParams, db) |. Future.mapOk(_ => x)
     );
};

let deleteOneById = (baseQuery, table, decoder, id, db) => {
  let sql =
    Delete.(
      make() |. from(table) |. where({j|AND $table.`id` = ?|j}) |. toSql
    );
  let params = [|Sql.Id.toJson @@ id|] |. Params.positional;
  let errorMsg = "ERROR: deleteOneById failed";

  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(x =>
       mutate("deleteOneById", sql, params, db) |. Future.mapOk(_ => x)
     );
};

let incrementOneById = (baseQuery, table, decoder, field, id, db) => {
  let sql =
    Update.(
      make()
      |. from(table)
      |. set(field, {j|$field + 1|j})
      |. where({j|AND $table.`id` = ?|j})
      |. toSql
    );
  let params = [|Sql.Id.toJson @@ id|] |> PimpMySql_Params.positional;
  let errorMsg = "ERROR: incrementOneById failed";
  getOneById(baseQuery, table, decoder, id, db)
  |. Future.flatMapOk(x => assertHasItem(errorMsg, x) |. Future.value)
  |. Future.flatMapOk(_ => mutate("incrementOneByid", sql, params, db))
  |. Future.flatMapOk(_ => getOneById(baseQuery, table, decoder, id, db));
};
