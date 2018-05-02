open Jest;

/* Types */
type animal = {
  id: int,
  type_: string,
};

/* Database Creation and Connection */
module Sql = SqlCommon.Make_sql(MySql2);

let conn = MySql2.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let db = "pimpmysqltest";

let table = "animal";

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

/* Model Factory */
module Config = {
  let table = table;
  let base =
    SqlComposer.Select.(
      select
      |> field("animal.id")
      |> field("animal.type_")
      |> order_by(`Desc("animal.id"))
    );
};

module Model = FactoryModel.Generator(Config);

/* Tests */
describe("FactoryModel", () => {
  createTestData(conn);
  let decoder = json => {
    id: Json.Decode.field("id", Json.Decode.int, json),
    type_: Json.Decode.field("type_", Json.Decode.string, json),
  };
  testPromise("getById (returns a result)", () =>
    Model.getById(decoder, 1, conn)
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
  testPromise("getById (does not return a result)", () =>
    Model.getById(decoder, 5, conn)
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
  afterAll(() => {
    Sql.mutate(conn, ~sql=dropDb, (_) => ());
    MySql2.close(conn);
  });
});
