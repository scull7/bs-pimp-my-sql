open Jest;
open SqlComposer;

/* Types */
type animalExternal = {
  id: int,
  type_: string,
  deleted: int,
  deleted_timestamp: int,
};

type animalInternal = {type_: string};

type person = {
  id: int,
  first_name: string,
  active: int,
  deleted: int,
};

let logAndFail = (label, x) => {
  Js.log2({j|ERROR: $label -|j}, x);
  fail("not an expected response");
};

let logAndFailAsync = (label, x, finish) => logAndFail(label, x) |. finish;

/* Database Creation and Connection */
module Sql = SqlCommon.Make_sql(MySql2);

let conn = Sql.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let db = "pimpmysqlquery";

let table = "animals";

let table2 = "people";

let createDb = {j|CREATE DATABASE $db;|j};

let useDB = {j|USE $db;|j};

let dropDb = {j|DROP DATABASE $db;|j};

let createTable = {j|
  CREATE TABLE $table (
    id MEDIUMINT NOT NULL AUTO_INCREMENT,
    type_ VARCHAR(120) NOT NULL,
    deleted TINYINT(1) NOT NULL DEFAULT 0,
    deleted_timestamp int(10) UNSIGNED NOT NULL DEFAULT 0,
    primary key (id),
    unique(type_)
  );
|j};

let createTable2 = {j|
  CREATE TABLE $table2 (
    id MEDIUMINT NOT NULL AUTO_INCREMENT,
    first_name VARCHAR(120) NOT NULL,
    active TINYINT(1) NOT NULL DEFAULT 1,
    deleted int(10) UNSIGNED NOT NULL DEFAULT 0,
    primary key (id)
  );
|j};

let seedTable = {j|
  INSERT INTO $table (type_)
  VALUES ('dog'), ('cat'), ('elephant');
|j};

let seedTable2 = {j|
  INSERT INTO $table2 (first_name)
  VALUES ('gayle'), ('patrick'), ('cody'), ('clinton');
|j};

let base = Select.(select |. field("*") |. from(table));

let base2 = Select.(select |. field("*") |. from(table2));

let createTestData = conn => {
  Sql.mutate(conn, ~sql=createDb, _ => ());
  Sql.mutate(conn, ~sql=useDB, _ => ());
  Sql.mutate(conn, ~sql=createTable, _ => ());
  Sql.mutate(conn, ~sql=seedTable, _ => ());
  Sql.mutate(conn, ~sql=createTable2, _ => ());
  Sql.mutate(conn, ~sql=seedTable2, _ => ());
};

/* Model Factory */
describe("PimpMySql_Query", () => {
  createTestData(conn);
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      type_: field("type_", string, json),
      deleted: field("deleted", int, json),
      deleted_timestamp: field("deleted_timestamp", int, json),
    };
  let decoder2 = json =>
    Json.Decode.{
      id: field("id", int, json),
      first_name: field("first_name", string, json),
      active: field("active", int, json),
      deleted: field("deleted", int, json),
    };
  testAsync("getOneById (returns 1 result)", finish =>
    PimpMySql_Query.getOneById(base, table, decoder, 3, conn)
    |. Future.mapOk(
         fun
         | Some({id: 3, type_: "elephant"}) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore
  );
  testAsync("getOneById (does not return anything)", finish =>
    PimpMySql_Query.getOneById(base, table, decoder, 4, conn)
    |. Future.mapOk(
         fun
         | Some(_) => fail("not an expected result") |. finish
         | None => pass |. finish,
       )
    |. ignore
  );
  /**
   * @TODO - bs-sql-common query_batch method needs to be implemented before
   *         this will work.
   */
  /*
   Only.testAsync("getByIdList (returns 3 results)", (finish) =>
     PimpMySql_Query.getByIdList(base, table, decoder, [1, 2], conn)
     |. Future.mapOk(
       /**
        * @TODO: there is a bug with mysql2, once fixed add
        * fail("expected to get 2 results back") back to the catchall
        */
       fun
       | [|{id: 1, type_: "dog"}, {id: 2, type_: "cat"}|] => pass |. finish
       | _ => pass |. finish
     )
     |. Future.mapError(e => { Js.log(e); fail("got error") |. finish })
     |. ignore
   );
   testAsync("getByIdList (does not return anything)", (finish) =>
     PimpMySql_Query.getByIdList(base, table, decoder, [6, 7, 8], conn)
     |. Future.mapOk(
       fun
       | [||] => pass |. finish
       | _ => fail("not an expected result") |. finish
     )
     |. ignore
   );
   */
  testAsync("getOneBy (returns 1 result)", finish => {
    let sql = Select.(base |. where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("elephant")|]);
    PimpMySql_Query.getOneBy(sql, decoder, params, conn)
    |. Future.mapOk(
         fun
         | Some({id: 3, type_: "elephant"}) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("getOneBy (does not return anything)", finish => {
    let sql = Select.(base |. where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("groundhog")|]);
    PimpMySql_Query.getOneBy(sql, decoder, params, conn)
    |. Future.mapOk(
         fun
         | Some(_) => fail("not an expected result") |. finish
         | None => pass |. finish,
       )
    |. ignore;
  });
  testAsync("get (returns 1 result)", finish => {
    let sql = Select.(base |. where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("elephant")|]);
    PimpMySql_Query.get(sql, decoder, params, conn)
    |. Future.mapOk(
         fun
         | [|{id: 3, type_: "elephant"}|] => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("get (does not return anything)", finish => {
    let sql = Select.(base |. where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("groundhog")|]);
    PimpMySql_Query.get(sql, decoder, params, conn)
    |. Future.mapOk(
         fun
         | [||] => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("getWhere (returns 1 result)", finish => {
    let where = [{j|AND $table.`type_` = ?|j}];
    let params = Json.Encode.([|string("elephant")|]);
    PimpMySql_Query.getWhere(base, where, decoder, params, conn)
    |. Future.mapOk(
         fun
         | [|{id: 3, type_: "elephant"}|] => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("getWhere (does not return anything)", finish => {
    let where = [{j|AND $table.`type_` = ?|j}];
    let params = Json.Encode.([|string("groundhog")|]);
    PimpMySql_Query.getWhere(base, where, decoder, params, conn)
    |. Future.mapOk(
         fun
         | [||] => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("getWhere (fails and throws syntax error exception)", finish => {
    let where = [{j|$table.`type_` = ?|j}];
    let params = Json.Encode.([|string("elephant")|]);
    PimpMySql_Query.getWhere(base, where, decoder, params, conn)
    |. Future.mapOk(_ => fail("not an expected result") |. finish)
    |. Future.mapError(_ => pass |. finish)
    |. ignore;
  });
  testAsync("insertOne (returns 1 result)", finish => {
    let record = {type_: "pangolin"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.insertOne(base, table, decoder, encoder, record, conn)
    |. Future.mapOk(
         fun
         | Some({id: 4, type_: "pangolin"}) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("insertOne (succeeds but returns no result)", finish => {
    let base = base |. Select.where({j|AND $table.`deleted` = 0|j});
    let record = {type_: "turkey"};
    let encoder = x =>
      [
        ("type_", Json.Encode.string @@ x.type_),
        ("deleted", Json.Encode.int @@ 1),
      ]
      |> Json.Encode.object_;
    PimpMySql_Query.insertOne(base, table, decoder, encoder, record, conn)
    |. Future.mapOk(
         fun
         | None => pass |. finish
         | Some(_) => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("insertOne (fails and throws unique constraint error)", fin => {
    let record = {type_: "elephant"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.insertOne(base, table, decoder, encoder, record, conn)
    |. Future.mapOk(_ => fail("not an expected result") |. fin)
    |. Future.mapError(_ => pass |. fin)
    |. ignore;
  });
  testAsync("insertOne (fails and throws bad field error)", finish => {
    let record = {type_: "flamingo"};
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.insertOne(base, table, decoder, encoder, record, conn)
    |. Future.mapOk(_ => fail("not an expected result") |. finish)
    |. Future.mapError(_ => pass |. finish)
    |. ignore;
  });
  testAsync("insertBatch (returns 2 results)", finish => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    PimpMySql_Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder,
      ~loader=animals => Future.value(Belt.Result.Ok(animals)),
      ~error=msg => PimpMySql_Error.MutationFailure(msg),
      ~columns=[|"type_"|],
      ~rows=[|{type_: "catfish"}, {type_: "lumpsucker"}|],
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Ok([|{type_: "catfish"}, {type_: "lumpsucker"}|]) =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("insertBatch (fails and throws unique constraint error)", f => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    PimpMySql_Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder,
      ~loader=animals => Future.value(Belt.Result.Ok(animals)),
      ~error=msg => PimpMySql_Error.MutationFailure(msg),
      ~columns=[|"type_"|],
      ~rows=[|{type_: "dog"}, {type_: "cat"}|],
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Error(_) => pass |. f
         | Belt.Result.Ok(_) => fail("not an expected result") |. f,
       )
    |. ignore;
  });
  testAsync("insertBatch (given empty array returns nothing)", finish => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    PimpMySql_Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder,
      ~loader=animals => Future.value(Belt.Result.Ok(animals)),
      ~error=msg => PimpMySql_Error.MutationFailure(msg),
      ~columns=[|"type_"|],
      ~rows=[||],
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Ok([||]) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("updateOneById (returns 1 result)", finish => {
    let record = {type_: "hamster"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.updateOneById(
      base,
      table,
      decoder,
      encoder,
      record,
      1,
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Ok(Some({id: 1, type_: "hamster"})) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("updateOneById (succeeds but returns no result)", finish => {
    let base = base |. Select.where({j|AND $table.`deleted` = 0|j});
    let record = {type_: "chicken"};
    let encoder = x =>
      [
        ("type_", Json.Encode.string @@ x.type_),
        ("deleted", Json.Encode.int @@ 1),
      ]
      |> Json.Encode.object_;
    PimpMySql_Query.updateOneById(
      base,
      table,
      decoder,
      encoder,
      record,
      1,
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Ok(None) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("updateOneById (fails and returns NotFound)", finish => {
    let record = {type_: "goose"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.updateOneById(
      base,
      table,
      decoder,
      encoder,
      record,
      9,
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.NotFound(_)) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("updateOneById (fails and throws bad field error)", finish => {
    let record = {type_: "hippopotamus"};
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.updateOneById(
      base,
      table,
      decoder,
      encoder,
      record,
      1,
      conn,
    )
    |. Future.mapOk(_ => fail("not an expected result") |. finish)
    |. Future.mapError(_ => pass |. finish)
    |. ignore;
  });
  testAsync("deactivateOneById (returns 1 result)", finish =>
    PimpMySql_Query.deactivateOneById(base2, table2, decoder2, 2, conn)
    |. Future.mapOk(
         fun
         | Some({id: 2, first_name: "patrick", active: 0, deleted: 0}) =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore
  );
  testAsync("deactivateOneById (succeeds but returns no result)", f => {
    let base2 = base2 |. Select.where({j|AND $table2.`active` = 1|j});
    PimpMySql_Query.deactivateOneById(base2, table2, decoder2, 1, conn)
    |. Future.mapOk(
         fun
         | None => pass |. f
         | _ => fail("not an expected result") |. f,
       )
    |. ignore;
  });
  testAsync("deactivateOneById (fails and returns NotFound)", finish =>
    PimpMySql_Query.deactivateOneById(base2, table2, decoder2, 99, conn)
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.NotFound(_)) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore
  );
  testAsync("archiveOneById (returns 1 result)", finish =>
    PimpMySql_Query.archiveOneById(base2, table2, decoder2, 2, conn)
    |. Future.map(
         fun
         | Belt.Result.Ok(
             Some({id: 2, first_name: "patrick", active: 0, deleted: 0}),
           ) =>
           fail("not an expected result") |. finish
         | Belt.Result.Ok(Some({id: 2, first_name: "patrick", active: 0})) =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore
  );
  testAsync("archiveOneById (succeeds but returns no result)", finish => {
    let base2 = base2 |. Select.where({j|AND $table2.`deleted` = 0|j});
    PimpMySql_Query.archiveOneById(base2, table2, decoder2, 1, conn)
    |. Future.mapOk(
         fun
         | None => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("archiveOneById (fails and returns NotFound)", finish =>
    PimpMySql_Query.archiveOneById(base2, table2, decoder2, 99, conn)
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.NotFound(_)) => pass |. finish
         | x => logAndFailAsync("archiveOneById", x, finish),
       )
    |. ignore
  );
  testAsync("archiveCompoundBy (returns 1 result)", finish => {
    let where = [{j|AND $table.`type_` = ?|j}];
    let params = Json.Encode.([|string("catfish")|]);
    PimpMySql_Query.archiveCompoundBy(
      base,
      where,
      table,
      decoder,
      params,
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Ok([|
             {id: 7, type_: "catfish", deleted: 1, deleted_timestamp: 0},
           |]) =>
           fail("not an expected result") |. finish
         | Belt.Result.Ok([|{id: 7, type_: "catfish", deleted: 1}|]) =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("archiveCompoundBy (succeeds but returns no result)", finish => {
    let base = base |. Select.where({j|AND $table.`deleted` = 0|j});
    let where = [{j|AND $table.`type_` = ?|j}];
    let params = Json.Encode.([|string("lumpsucker")|]);
    PimpMySql_Query.archiveCompoundBy(
      base,
      where,
      table,
      decoder,
      params,
      conn,
    )
    |. Future.mapOk(
         fun
         | [||] => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync(
    "archiveCompoundBy (fails and returns UnexpectedEmptyArray)", finish => {
    let where = [{j|AND $table.`type_` = ?|j}];
    let params = Json.Encode.([|string("blahblahblah")|]);
    PimpMySql_Query.archiveCompoundBy(
      base,
      where,
      table,
      decoder,
      params,
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.UnexpectedEmptyArray(_)) =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("archiveCompoundBy (fails and returns EmptyUserQuery)", fin => {
    let where = [];
    let params = Json.Encode.([|string("blahblahblah")|]);
    PimpMySql_Query.archiveCompoundBy(
      base,
      where,
      table,
      decoder,
      params,
      conn,
    )
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.EmptyUserQuery(_)) => pass |. fin
         | _ => fail("not an expected result") |. fin,
       )
    |. ignore;
  });
  testAsync("archiveCompoundOneById (returns 1 result)", finish =>
    PimpMySql_Query.archiveCompoundOneById(base, table, decoder, 2, conn)
    |. Future.mapOk(
         fun
         | Some({id: 2, type_: "cat", deleted: 1, deleted_timestamp: 0} as x) =>
           logAndFailAsync("archiveCompoundOneById - 1", x, finish)
         | Some({id: 2, type_: "cat", deleted: 1}) => pass |. finish
         | x => logAndFailAsync("archiveCompoundOneById - 2", x, finish),
       )
    |. ignore
  );
  testAsync("archiveCompoundOneById (succeeds but returns no result)", f => {
    let base = base |. Select.where({j|AND $table.`deleted` = 0|j});
    PimpMySql_Query.archiveCompoundOneById(base, table, decoder, 3, conn)
    |. Future.mapOk(_ => pass |. f)
    |. Future.mapError(_ => fail("not an expected result") |. f)
    |. ignore;
  });
  testAsync("archiveCompoundOneById (fails and returns NotFound)", finish =>
    PimpMySql_Query.archiveCompoundOneById(base, table, decoder, 99, conn)
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.NotFound(_)) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore
  );
  testAsync("deleteBy (returns 2 result)", finish => {
    let where = [{j|AND $table2.`deleted` != ?|j}];
    let params = Json.Encode.([|int(0)|]);
    PimpMySql_Query.deleteBy(base2, where, table2, decoder2, params, conn)
    |. Future.mapOk(
         fun
         | [|{id: 1, first_name: "gayle"}, {id: 2, first_name: "patrick"}|] =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("deleteBy (fails and returns NotFound)", finish => {
    let where = [{j|AND $table2.`deleted` != ?|j}];
    let params = Json.Encode.([|int(0)|]);
    PimpMySql_Query.deleteBy(base2, where, table2, decoder2, params, conn)
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.UnexpectedEmptyArray(_)) =>
           pass |. finish
         | x => {
             Js.log2("ERROR: deleteBy", x);
             fail("not an expected result") |. finish;
           },
       )
    |. ignore;
  });
  testAsync("deleteBy (fails and returns EmptyUserQuery)", finish => {
    let where = [];
    let params = Json.Encode.([|int(0)|]);
    PimpMySql_Query.deleteBy(base2, where, table2, decoder2, params, conn)
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.EmptyUserQuery(_)) =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore;
  });
  testAsync("deleteOneById (returns 1 result)", finish =>
    PimpMySql_Query.deleteOneById(base, table, decoder, 3, conn)
    |. Future.map(
         fun
         | Belt.Result.Ok({
             id: 3,
             type_: "elephant",
             deleted: 1,
             deleted_timestamp: 0,
           }) =>
           fail("not an expected result") |. finish
         | Belt.Result.Ok({id: 3, type_: "elephant", deleted: 1}) =>
           pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore
  );
  testAsync("deleteOneById (fails and returns NotFound)", finish =>
    PimpMySql_Query.deleteOneById(base, table, decoder, 99, conn)
    |. Future.map(
         fun
         | Belt.Result.Error(PimpMySql_Error.NotFound(_)) => pass |. finish
         | _ => fail("not an expected result") |. finish,
       )
    |. ignore
  );
  afterAll(() => {
    Sql.mutate(conn, ~sql=dropDb, _ => ());
    MySql2.close(conn);
  });
});
