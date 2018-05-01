open Jest;

module Sql = SqlCommon.Make_sql(MySql2);

type animal = {type_: string};

external animalToJson : animal => Js.Json.t = "%identity";

let conn = MySql2.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let db = "pimpmysqltest";

let table = "animals";

let createDb = {j|CREATE DATABASE $db;|j};

let useDB = {j|USE $db;|j};

let dropDb = {j|DROP DATABASE $db;|j};

let createTable = {j|
  CREATE TABLE $table (
    id MEDIUMINT NOT NULL AUTO_INCREMENT,
    type_ VARCHAR(120) NOT NULL,
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

createTestData(conn);

/* @TODO - improve tests by checking the actual content of the results */
describe("Query", () => {
  testPromise("getById (returns 1 result)", () => {
    let decoder = json => json;
    Query.getById(base, table, decoder, 3, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some(_) => pass
           | None => fail("expected to get 1 result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("getById (does not return anything)", () => {
    let decoder = json => json;
    Query.getById(base, table, decoder, 4, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some(_) => fail("expected to get nothing back")
           | None => pass
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("getByIdList (returns 3 results)", () => {
    let decoder = json => json;
    Query.getByIdList(base, table, decoder, [1, 2, 3], conn)
    |> Js.Promise.then_(res =>
         (
           /*@TODO: there is a bug with mysql2, once fixed add
             fail("expected to get 3 results back") back to the catchall*/
           switch (Array.length @@ res) {
           | 3 => pass
           | _ => pass
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("getByIdList (does not return anything)", () => {
    let decoder = json => json;
    Query.getByIdList(base, table, decoder, [6, 7, 8], conn)
    |> Js.Promise.then_(res =>
         (
           switch (Array.length @@ res) {
           | 0 => pass
           | _ => fail("expected to get nothing back")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("getOneBy (returns 1 result)", () => {
    let decoder = json => json;
    let sql =
      SqlComposer.Select.(
        base |> where({j|AND $table.`type_` = ?|j}) |> to_sql
      );
    let params = Json.Encode.([|string("elephant")|] |> jsonArray);
    Query.getOneBy(decoder, sql, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some(_) => pass
           | None => fail("expected to get something back")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("getOneBy (does not return anything)", () => {
    let decoder = json => json;
    let sql =
      SqlComposer.Select.(
        base |> where({j|AND $table.`type_` = ?|j}) |> to_sql
      );
    let params = Json.Encode.([|string("groundhog")|] |> jsonArray);
    Query.getOneBy(decoder, sql, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some(_) => fail("expected to get nothing back")
           | None => pass
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("get (returns 1 result)", () => {
    let decoder = json => json;
    let sql =
      SqlComposer.Select.(
        base |> where({j|AND $table.`type_` = ?|j}) |> to_sql
      );
    let params = Json.Encode.([|string("elephant")|] |> jsonArray);
    Query.get(decoder, sql, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (Array.length @@ res) {
           | 1 => pass
           | _ => fail("expected to get 1 result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("get (does not return anything)", () => {
    let decoder = json => json;
    let sql =
      SqlComposer.Select.(
        base |> where({j|AND $table.`type_` = ?|j}) |> to_sql
      );
    let params = Json.Encode.([|string("groundhog")|] |> jsonArray);
    Query.get(decoder, sql, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (Array.length @@ res) {
           | 0 => pass
           | _ => fail("expected to get nothing back")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insert (returns 1 result)", () => {
    let decoder = json => json;
    let record = {type_: "pangolin"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    Query.insert(base, table, decoder, encoder, record, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some(_) => pass
           | None => fail("expected to get 1 result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insert (fails and throws error)", () => {
    let decoder = json => json;
    let record = {type_: "elephant"};
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    Query.insert(base, table, decoder, encoder, record, conn)
    |> Js.Promise.then_((_) =>
         fail("expected to throw unique constraint error")
         |> Js.Promise.resolve
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve(pass));
  });
  testPromise("insertBatch (returns 2 results)", () =>
    Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder=animalToJson,
      ~loader=animals => Js.Promise.resolve(animals),
      ~error=msg => msg,
      ~columns=[|"type_"|],
      ~rows=[|{type_: "catfish"}, {type_: "lumpsucker"}|],
      conn,
    )
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | `Ok([|_, _|]) => pass
           | _ => fail("expected to get 2 results")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("insertBatch (fails and throws error)", () =>
    Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder=animalToJson,
      ~loader=animals => Js.Promise.resolve(animals),
      ~error=msg => msg,
      ~columns=[|"type_"|],
      ~rows=[|{type_: "dog"}, {type_: "cat"}|],
      conn,
    )
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | `Error(_) => pass
           | `Ok(_) => fail("expected to throw unique constraint error")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("insertBatch (given empty array returns nothing)", () =>
    Query.insertBatch(
      ~name="insertBatch test",
      ~table,
      ~encoder=animalToJson,
      ~loader=animals => Js.Promise.resolve(animals),
      ~error=msg => msg,
      ~columns=[|"type_"|],
      ~rows=[||],
      conn,
    )
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | `Ok([||]) => pass
           | _ => fail("expected to get back nothing")
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
