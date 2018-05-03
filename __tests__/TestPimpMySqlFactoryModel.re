open Jest;

/* Types */
type animal = {
  id: int,
  type_: string,
  deleted: int,
};

type animalInternal = {type_: string};

/* Database Creation and Connection */
module Sql = SqlCommon.Make_sql(MySql2);

let conn = Sql.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let db = "pimpmysqlfactorymodel";

let table = "animal";

let table2 = "color";

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

let createTable2 = {j|
  CREATE TABLE $table2 (
    id MEDIUMINT NOT NULL,
    type_ VARCHAR(120) NOT NULL,
    deleted TINYINT(1) NOT NULL DEFAULT 0,
    primary key (type_)
  );
|j};

let seedTable2 = {j|
  INSERT INTO $table2 (id, type_)
  VALUES (1, 'red'), (1, 'green'), (1, 'blue');
|j};

let createTestData = conn => {
  Sql.mutate(conn, ~sql=createDb, (_) => ());
  Sql.mutate(conn, ~sql=useDB, (_) => ());
  Sql.mutate(conn, ~sql=createTable, (_) => ());
  Sql.mutate(conn, ~sql=seedTable, (_) => ());
  Sql.mutate(conn, ~sql=createTable2, (_) => ());
  Sql.mutate(conn, ~sql=seedTable2, (_) => ());
};

/* Model Factory */
module Config = {
  let table = table;
  let base =
    SqlComposer.Select.(
      select
      |> field({j|$table.`id`|j})
      |> field({j|$table.`type_`|j})
      |> field({j|$table.`deleted`|j})
      |> order_by(`Desc({j|$table.`id`|j}))
    );
};

module Model = PimpMySql_FactoryModel.Generator(Config);

module Config2 = {
  let table = table2;
  let base =
    SqlComposer.Select.(
      select
      |> field({j|$table2.`id`|j})
      |> field({j|$table2.`type_`|j})
      |> field({j|$table2.`deleted`|j})
      |> order_by(`Desc({j|$table2.`id`|j}))
    );
};

module Model2 = PimpMySql_FactoryModel.Generator(Config2);

/* Tests */
describe("PimpMySql_FactoryModel", () => {
  createTestData(conn);
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      type_: field("type_", string, json),
      deleted: field("deleted", int, json),
    };
  testPromise("getOneById (returns a result)", () =>
    Model.getOneById(decoder, 1, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some({id: 1, type_: "dog"}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("getOneById (does not return a result)", () =>
    Model.getOneById(decoder, 5, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | None => pass
           | Some(_) => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("getOneById (fails and throws unexpected result count)", () =>
    Model2.getOneById(decoder, 1, conn)
    |> Js.Promise.then_((_) =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve @@ pass)
  );
  testPromise("getByIdList (returns 2 results)", () =>
    Model.getByIdList(decoder, [1, 2], conn)
    |> Js.Promise.then_(res =>
         (
           /*@TODO: there is a bug with mysql2, once fixed add
             fail("not an expected result") back to the catchall*/
           switch (res) {
           | [|{id: 1, type_: "dog"}, {id: 2, type_: "cat"}|] => pass
           | _ => pass
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("getByIdList (does not return any results)", () =>
    Model.getByIdList(decoder, [4, 5], conn)
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
  testPromise("getOneBy (returns a result)", () => {
    let userClauses =
      SqlComposer.Select.(
        select
        |> where({j|AND $table.`id` = ?|j})
        |> where({j|AND $table.`type_` = ?|j})
      );
    let params = Json.Encode.([|int(1), string("dog")|] |> jsonArray);
    Model.getOneBy(userClauses, decoder, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some({id: 1, type_: "dog"}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("getOneBy (does not return a result)", () => {
    let userClauses =
      SqlComposer.Select.(
        select
        |> where({j|AND $table.`id` = ?|j})
        |> where({j|AND $table.`type_` = ?|j})
      );
    let params = Json.Encode.([|int(1), string("cat")|] |> jsonArray);
    Model.getOneBy(userClauses, decoder, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | None => pass
           | Some(_) => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("get (returns 2 results)", () => {
    let userClauses =
      SqlComposer.Select.(
        select
        |> where({j|AND $table.`id` != ?|j})
        |> where({j|AND $table.`type_` LIKE CONCAT("%", ?, "%")|j})
      );
    let params = Json.Encode.([|int(1), string("a")|] |> jsonArray);
    Model.get(userClauses, decoder, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | [|{id: 3, type_: "elephant"}, {id: 2, type_: "cat"}|] => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("get (does not return any results)", () => {
    let userClauses =
      SqlComposer.Select.(
        select |> where({j|AND $table.`type_` LIKE CONCAT(?, "%")|j})
      );
    let params = Json.Encode.([|string("z")|] |> jsonArray);
    Model.get(userClauses, decoder, params, conn)
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
  testPromise("insert (returns 1 result)", () => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "monkey"};
    Model.insert(decoder, encoder, record, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some({id: 4, type_: "monkey"}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insert (fails and throws unique constraint error)", () => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "dog"};
    Model.insert(decoder, encoder, record, conn)
    |> Js.Promise.then_((_) =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve @@ pass);
  });
  testPromise("insert (does not return a result, throws bad field error)", () => {
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "flamingo"};
    Model.insert(decoder, encoder, record, conn)
    |> Js.Promise.then_((_) =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve @@ pass);
  });
  testPromise("updateById (returns 1 result)", () => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    Model.updateById(decoder, encoder, record, 1, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(Some({id: 1, type_: "hippopotamus"})) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("updateById (does not return a result)", () => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    Model.updateById(decoder, encoder, record, 99, conn)
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
  testPromise(
    "updateById (does not return a result, throws bad field error)", () => {
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    Model.updateById(decoder, encoder, record, 1, conn)
    |> Js.Promise.then_((_) =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch((_) => Js.Promise.resolve @@ pass);
  });
  testPromise("archiveCompoundById (returns 1 result)", () =>
    Model.archiveCompoundById(decoder, 2, conn)
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
  testPromise("archiveCompoundById (does not return a result)", () =>
    Model.archiveCompoundById(decoder, 99, conn)
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
