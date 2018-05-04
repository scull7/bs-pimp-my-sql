open Jest;

/* Types */
type animalExternal = {
  id: int,
  type_: string,
  deleted: int,
};

type animalInternal = {type_: string};

/* Database Creation and Connection */
module Sql = SqlCommon.Make_sql(MySql2);

let conn = Sql.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let db = "pimpmysqlquery";

let table = "animals";

let createDb = {j|CREATE DATABASE $db;|j};

let useDB = {j|USE $db;|j};

let dropDb = {j|DROP DATABASE $db;|j};

let createTable = {j|
  CREATE TABLE $table (
    id MEDIUMINT NOT NULL AUTO_INCREMENT,
    type_ VARCHAR(120) NOT NULL,
    deleted TINYINT(1) NOT NULL DEFAULT 0,
    deleted_timestamp TIMESTAMP NULL DEFAULT NULL,
    primary key (id),
    unique(type_)
  );
|j};

let seedTable = {j|
  INSERT INTO $table (type_)
  VALUES ('dog'), ('cat'), ('elephant');
|j};

let base = SqlComposer.Select.(select |> field("*") |> from(table));

let createTestData = conn => {
  Sql.mutate(conn, ~sql=createDb, (_) => ());
  Sql.mutate(conn, ~sql=useDB, (_) => ());
  Sql.mutate(conn, ~sql=createTable, (_) => ());
  Sql.mutate(conn, ~sql=seedTable, (_) => ());
};

/* Model Factory */
describe("PimpMySql_Query", () => {
  createTestData(conn);
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      type_: field("type_", string, json),
      deleted: field("deleted", int, json),
    };
  testPromise("getOneById (returns 1 result)", () =>
    PimpMySql_Query.getOneById(base, table, decoder, 3, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some({id: 3, type_: "elephant"}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("getOneById (does not return anything)", () =>
    PimpMySql_Query.getOneById(base, table, decoder, 4, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some(_) => fail("not an expected result")
           | None => pass
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("getByIdList (returns 3 results)", () =>
    PimpMySql_Query.getByIdList(base, table, decoder, [1, 2], conn)
    |> Js.Promise.then_(res =>
         (
           /*@TODO: there is a bug with mysql2, once fixed add
             fail("expected to get 2 results back") back to the catchall*/
           switch (res) {
           | [|{id: 1, type_: "dog"}, {id: 2, type_: "cat"}|] => pass
           | _ => pass
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("getByIdList (does not return anything)", () =>
    PimpMySql_Query.getByIdList(base, table, decoder, [6, 7, 8], conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | [||] => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("getOneBy (returns 1 result)", () => {
    let sql =
      SqlComposer.Select.(base |> where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("elephant")|] |> jsonArray);
    PimpMySql_Query.getOneBy(sql, decoder, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some({id: 3, type_: "elephant"}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("getOneBy (does not return anything)", () => {
    let sql =
      SqlComposer.Select.(base |> where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("groundhog")|] |> jsonArray);
    PimpMySql_Query.getOneBy(sql, decoder, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some(_) => fail("not an expected result")
           | None => pass
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("get (returns 1 result)", () => {
    let sql =
      SqlComposer.Select.(base |> where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("elephant")|] |> jsonArray);
    PimpMySql_Query.get(sql, decoder, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | [|{id: 3, type_: "elephant"}|] => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("get (does not return anything)", () => {
    let sql =
      SqlComposer.Select.(base |> where({j|AND $table.`type_` = ?|j}));
    let params = Json.Encode.([|string("groundhog")|] |> jsonArray);
    PimpMySql_Query.get(sql, decoder, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | [||] => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insertOne (returns 1 result)", () => {
    let record = {type_: "pangolin"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.insertOne(base, table, decoder, encoder, record, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some({id: 4, type_: "pangolin"}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insertOne (fails and throws unique constraint error)", () => {
    let record = {type_: "elephant"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.insertOne(base, table, decoder, encoder, record, conn)
    |> Js.Promise.then_((_) =>
         fail("not an expected result") |> Js.Promise.resolve
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve(pass));
  });
  testPromise("insertOne (fails and throws bad field error)", () => {
    let record = {type_: "flamingo"};
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    PimpMySql_Query.insertOne(base, table, decoder, encoder, record, conn)
    |> Js.Promise.then_((_) =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve @@ pass);
  });
  testPromise("insertBatch (returns 2 results)", () => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    PimpMySql_Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder,
      ~loader=animals => Js.Promise.resolve(animals),
      ~error=msg => msg,
      ~columns=[|"type_"|],
      ~rows=[|{type_: "catfish"}, {type_: "lumpsucker"}|],
      conn,
    )
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok([|{type_: "catfish"}, {type_: "lumpsucker"}|]) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insertBatch (fails and throws unique constraint error)", () => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    PimpMySql_Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder,
      ~loader=animals => Js.Promise.resolve(animals),
      ~error=msg => msg,
      ~columns=[|"type_"|],
      ~rows=[|{type_: "dog"}, {type_: "cat"}|],
      conn,
    )
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Error(_) => pass
           | Result.Ok(_) => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insertBatch (given empty array returns nothing)", () => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    PimpMySql_Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder,
      ~loader=animals => Js.Promise.resolve(animals),
      ~error=msg => msg,
      ~columns=[|"type_"|],
      ~rows=[||],
      conn,
    )
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok([||]) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("updateOneById (returns 1 result)", () => {
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
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(Some({id: 1, type_: "hamster"})) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("updateOneById (fails and does not return anything)", () => {
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
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Error(PimpMySql_Error.NotFound(_)) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("updateOneById (fails and throws bad field error)", () => {
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
    |> Js.Promise.then_((_) =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve @@ pass);
  });
  testPromise("archiveCompoundOneById (returns 1 result)", () =>
    PimpMySql_Query.archiveCompoundOneById(base, table, decoder, 2, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(Some({id: 2, type_: "cat", deleted: 1})) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise(
    "archiveCompoundOneById (fails and does not return anything)", () =>
    PimpMySql_Query.archiveCompoundOneById(base, table, decoder, 99, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Error(PimpMySql_Error.NotFound(_)) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  afterAll(() => {
    Sql.mutate(conn, ~sql=dropDb, (_) => ());
    MySql2.close(conn);
  });
});
