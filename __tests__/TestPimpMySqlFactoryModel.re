open Jest;

exception InsertBatchFailed(string);

[@bs.module "util"] external inspect : 'a => string = "";

let logAndFailAsync = TestPimpMySqlQuery.logAndFailAsync;

let toError = x => x |. inspect |. failwith |. Belt.Result.Error;

let success = Belt.Result.Ok(true);

let andThenTest = (future, label, finish, test) =>
  future
  |. Future.flatMapOk(x => x |. test |. Future.value)
  |. Future.mapOk(_ => pass |. finish)
  |. Future.mapError(x => logAndFailAsync(label, x, finish))
  |. ignore;

let shouldFail = (future, label, finish) =>
  future
  |. Future.mapOk(x => logAndFailAsync(label, x, finish))
  |. Future.mapError(_ => pass |. finish)
  |. ignore;

/* Types */
type animal = {
  id: int,
  type_: string,
  deleted: int,
  deleted_timestamp: int,
};

type animalInternal = {type_: string};

type person = {
  id: int,
  first_name: string,
  age: int,
  active: int,
  deleted: int,
};

/* Database Creation and Connection */
module Sql = SqlCommon.Make_sql(MySql2);

let conn = Sql.connect(~host="127.0.0.1", ~port=3306, ~user="root", ());

let db = "pimpmysqlfactorymodel";

let table = "animal_model";

let table2 = "person_model";

let createDb = {j|CREATE DATABASE $db;|j};

let useDB = {j|USE $db;|j};

let dropDb = {j|DROP DATABASE IF EXISTS $db;|j};

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
    age TINYINT(3) UNSIGNED NOT NULL,
    active TINYINT(1) NOT NULL DEFAULT 1,
    deleted int(10) UNSIGNED NOT NULL DEFAULT 0,
    primary key (id)
  );
|j};

let seedTable = {j|
   INSERT INTO $table (type_)
   VALUES ('dog'), ('cat'), ('elephant'), ('dogfish'), ('moose');
 |j};

let seedTable2 = {j|
  INSERT INTO $table2 (first_name, age)
  VALUES ('gayle', 28), ('patrick', 65), ('cody', 29), ('clinton', 40), ('jason', 35), ('jon', 12);
|j};

let base = SqlComposer.Select.(make() |. field("*") |. from(table));

let debug = Debug.make("bs-pimp-my-sql", "TEST:PimpMySql:FactoryModel");

let createTestData = conn => {
  let mutate = (str, sql) => {
    debug(str);
    Sql.Promise.mutate(conn, ~sql, ());
  };

  mutate({j| drop database - $db|j}, dropDb)
  |> Js.Promise.then_(_ => mutate({j| create database - $db|j}, createDb))
  |> Js.Promise.then_(_ => mutate({j|use database - $db|j}, useDB))
  |> Js.Promise.then_(_ => mutate({j|create table - $table|j}, createTable))
  |> Js.Promise.then_(_ => mutate({j|seed table - $table|j}, seedTable))
  |> Js.Promise.then_(_ =>
       mutate({j|create table - $table2|j}, createTable2)
     )
  |> Js.Promise.then_(_ => mutate({j|seed table - $table2|j}, seedTable2));
};

/* Model Factory */
module AnimalConfig = {
  type t = animal;
  let table = table;
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      type_: field("type_", string, json),
      deleted: field("deleted", int, json),
      deleted_timestamp: field("deleted_timestamp", int, json),
    };
  let base =
    SqlComposer.Select.(
      make()
      |. field({j|$table.id|j})
      |. field({j|$table.type_|j})
      |. field({j|$table.deleted|j})
      |. field({j|$table.deleted_timestamp|j})
      |. orderBy(`Desc({j|$table.id|j}))
    );
};

module AnimalConfig2 = {
  type t = animal;
  let table = table;
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      type_: field("type_", string, json),
      deleted: field("deleted", int, json),
      deleted_timestamp: field("deleted_timestamp", int, json),
    };
  let base =
    SqlComposer.Select.(
      make()
      |. field({j|$table.id|j})
      |. field({j|$table.type_|j})
      |. field({j|$table.deleted|j})
      |. field({j|$table.deleted_timestamp|j})
      |. where({j|AND $table.deleted = 0|j})
      |. orderBy(`Desc({j|$table.id|j}))
    );
};

module PersonConfig = {
  type t = person;
  let connection = conn;
  let table = table2;
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      first_name: field("first_name", string, json),
      age: field("age", int, json),
      active: field("active", int, json),
      deleted: field("deleted", int, json),
    };
  let base =
    SqlComposer.Select.(
      make()
      |. field({j|$table.id|j})
      |. field({j|$table.first_name|j})
      |. field({j|$table.age|j})
      |. field({j|$table.active|j})
      |. field({j|$table.deleted|j})
    );
};

module PersonConfig2 = {
  type t = person;
  let connection = conn;
  let table = table2;
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      first_name: field("first_name", string, json),
      age: field("age", int, json),
      active: field("active", int, json),
      deleted: field("deleted", int, json),
    };
  let base =
    SqlComposer.Select.(
      make()
      |. field({j|$table.id|j})
      |. field({j|$table.first_name|j})
      |. field({j|$table.age|j})
      |. field({j|$table.active|j})
      |. field({j|$table.deleted|j})
      |. where({j|AND $table.deleted = 0|j})
      |. where({j|AND $table.active = 1|j})
      |. where({j|AND $table2.age != 36|j})
    );
};

module AnimalModel = PimpMySql_FactoryModel.Generator(AnimalConfig);

module AnimalModel2 = PimpMySql_FactoryModel.Generator(AnimalConfig2);

module PersonModel = PimpMySql_FactoryModel.Generator(PersonConfig);

module PersonModel2 = PimpMySql_FactoryModel.Generator(PersonConfig2);

/* Tests */
describe("PimpMySql_FactoryModel", () => {
  beforeAllPromise(() => createTestData(conn));

  afterAllPromise(() =>
    Sql.Promise.mutate(conn, ~sql=dropDb, ())
    |> Js.Promise.then_(_ => MySql2.close(conn) |> Js.Promise.resolve)
  );

  testAsync("getOneById (returns a result)", finish =>
    AnimalModel.getOneById(1, conn)
    |. andThenTest(
         "getOneById",
         finish,
         fun
         | Some({id: 1, type_: "dog"}) => success
         | x => toError(x),
       )
  );
  testAsync("getOneById (does not return a result)", finish =>
    AnimalModel.getOneById(99, conn)
    |. andThenTest(
         "getOneById",
         finish,
         fun
         | None => success
         | Some(x) => toError(x),
       )
  );

  /**
    * @TODO - bs-sql-common query_batch method needs to be implemented before
    *         this will work.
    */
  /*
   testAsync("getByIdList (returns 2 results)", finish =>
     AnimalModel.getByIdList([1, 2], conn)
     |. andThenTest("getByIdList", finish,
       /**
        * @TODO: there is a bug with mysql2, once fixed add
        * fail("not an expected result") back to the catchall
        */
       fun
       | [|{id: 1, type_: "dog"}, {id: 2, type_: "cat"}|] => success
       | _ => success
     )
   );

   testAsync("getByIdList (does not return any results)", finish =>
     AnimalModel.getByIdList([98, 99], conn)
     |. andThenTest("getByIdList", finish,
       fun
       | [||] => success
       | x => toError(x)
     )
   );
   */
  testAsync("getOneBy (returns a result)", finish => {
    let userClauses = base =>
      SqlComposer.Select.(
        base
        |. where({j|AND $table.id = ?|j})
        |. where({j|AND $table.type_ = ?|j})
      );
    let params = Json.Encode.([|int(1), string("dog")|]);
    AnimalModel.getOneBy(userClauses, params, conn)
    |. andThenTest(
         "getOneBy",
         finish,
         fun
         | Some({id: 1, type_: "dog"}) => success
         | x => toError(x),
       );
  });

  testAsync("getOneBy (does not return a result)", finish => {
    let userClauses = base =>
      SqlComposer.Select.(
        base
        |. where({j|AND $table.id = ?|j})
        |. where({j|AND $table.type_ = ?|j})
      );
    let params = Json.Encode.([|int(1), string("cat")|]);
    AnimalModel.getOneBy(userClauses, params, conn)
    |. andThenTest(
         "getOneBy",
         finish,
         fun
         | None => success
         | Some(x) => toError(x),
       );
  });

  testAsync("get (returns 2 results)", finish => {
    let userClauses = base =>
      SqlComposer.Select.(
        base
        |. where({j|AND $table.id != ?|j})
        |. where({j|AND $table.type_ LIKE CONCAT("%", ?, "%")|j})
      );
    let params = Json.Encode.([|int(1), string("a")|]);
    AnimalModel.get(userClauses, params, conn)
    |. andThenTest(
         "getOneBy",
         finish,
         fun
         | [|{id: 3, type_: "elephant"}, {id: 2, type_: "cat"}|] => success
         | x => toError(x),
       );
  });

  testAsync("get (does not return any results)", finish => {
    let userClauses = base =>
      SqlComposer.Select.(
        base |. where({j|AND $table.type_ LIKE CONCAT(?, "%")|j})
      );
    let params = Json.Encode.([|string("z")|]);
    AnimalModel.get(userClauses, params, conn)
    |. andThenTest(
         "getOneBy",
         finish,
         fun
         | [||] => success
         | x => toError(x),
       );
  });

  testAsync("getWhere (returns 1 result)", finish => {
    let userClauses = base =>
      SqlComposer.Select.(base |. where({j|AND $table.type_ = ?|j}));
    let params = Json.Encode.([|string("elephant")|]);
    AnimalModel.getWhere(userClauses, params, conn)
    |. andThenTest(
         "getOneBy",
         finish,
         fun
         | [|{id: 3, type_: "elephant"}|] => success
         | x => toError(x),
       );
  });

  testAsync("getWhere (does not return any results)", finish => {
    let userClauses = base =>
      SqlComposer.Select.(base |. where({j|AND $table.type_ = ?|j}));
    let params = Json.Encode.([|string("mouse")|]);
    AnimalModel.getWhere(userClauses, params, conn)
    |. andThenTest(
         "getOneBy",
         finish,
         fun
         | [||] => success
         | x => toError(x),
       );
  });
  testAsync("getWhere (fails and throws syntax error exception)", finish => {
    let userClauses = base =>
      SqlComposer.Select.(base |. where({j|AND $table.type_ ?|j}));
    let params = Json.Encode.([|string("mouse")|]);
    AnimalModel.getWhere(userClauses, params, conn)
    |. shouldFail("getWhere", finish);
  });

  testAsync("insertOne (returns 1 result)", finish => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "monkey"};
    AnimalModel.insertOne(encoder, record, conn)
    |. andThenTest(
         "insertOne",
         finish,
         fun
         | Some({id: 6, type_: "monkey"}) => success
         | x => toError(x),
       );
  });

  testAsync("insertOne (succeeds but returns no result)", finish => {
    let encoder = x =>
      [
        ("type_", Json.Encode.string @@ x.type_),
        ("deleted", Json.Encode.int @@ 1),
      ]
      |> Json.Encode.object_;
    let record = {type_: "turkey"};
    AnimalModel2.insertOne(encoder, record, conn)
    |. andThenTest(
         "insertOne",
         finish,
         fun
         | None => success
         | Some(x) => toError(x),
       );
  });

  testAsync("insertOne (fails and throws unique constraint error)", finish => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "dog"};
    AnimalModel.insertOne(encoder, record, conn)
    |. shouldFail("insertOne", finish);
  });

  testAsync(
    "insertOne (does not return a result, throws bad field error)", finish => {
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "flamingo"};
    AnimalModel.insertOne(encoder, record, conn)
    |. shouldFail("insertOne", finish);
  });

  testAsync("insertBatch (returns 2 results)", finish => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    let loader = animals => Belt.Result.Ok(animals) |. Future.value;
    let error = msg => InsertBatchFailed(msg);
    let columns = [|"type_"|];
    let rows = [|{type_: "catfish"}, {type_: "lumpsucker"}|];
    AnimalModel.insertBatch(
      "insertBatch test",
      encoder,
      loader,
      error,
      columns,
      rows,
      conn,
    )
    |. andThenTest(
         "insertBatch",
         finish,
         fun
         | [|{type_: "catfish"}, {type_: "lumpsucker"}|] => success
         | x => toError(x),
       );
  });

  testAsync("insertBatch (fails and throws unique constraint error)", finish => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    let loader = animals => Future.value(Belt.Result.Ok(animals));
    let error = msg => InsertBatchFailed(msg);
    let columns = [|"type_"|];
    let rows = [|{type_: "dog"}, {type_: "cat"}|];
    AnimalModel.insertBatch(
      "insertBatch test",
      encoder,
      loader,
      error,
      columns,
      rows,
      conn,
    )
    |. shouldFail("insertBatch", finish);
  });

  testAsync("insertBatch (given empty array returns nothing)", finish => {
    let encoder = x => Json.Encode.string @@ x.type_;
    let loader = animals => Future.value(Belt.Result.Ok(animals));
    let error = msg => InsertBatchFailed(msg);
    let columns = [|"type_"|];
    let rows = [||];
    AnimalModel.insertBatch(
      "insertBatch test",
      encoder,
      loader,
      error,
      columns,
      rows,
      conn,
    )
    |. andThenTest(
         "insertBatch",
         finish,
         fun
         | [||] => success
         | x => toError(x),
       );
  });

  testAsync("updateOneById (returns 1 result)", finish => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    AnimalModel.updateOneById(encoder, record, 1, conn)
    |. andThenTest(
         "insertBatch",
         finish,
         fun
         | Some({id: 1, type_: "hippopotamus"}) => success
         | x => toError(x),
       );
  });

  testAsync("updateOneById (succeeds but returns no result)", finish => {
    let encoder = x =>
      [
        ("type_", Json.Encode.string @@ x.type_),
        ("deleted", Json.Encode.int @@ 1),
      ]
      |> Json.Encode.object_;
    let record = {type_: "chicken"};
    AnimalModel2.updateOneById(encoder, record, 1, conn)
    |. andThenTest(
         "insertBatch",
         finish,
         fun
         | None => success
         | x => toError(x),
       );
  });

  testAsync("updateOneById (fails and returns NotFound)", finish => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    AnimalModel.updateOneById(encoder, record, 99, conn)
    |. shouldFail("updateOneById", finish);
  });

  testAsync(
    "updateOneById (does not return a result, throws bad field error)", finish => {
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    AnimalModel.updateOneById(encoder, record, 1, conn)
    |. shouldFail("updateOneById", finish);
  });

  testAsync("deactivateOneById (returns 1 result)", finish =>
    PersonModel.deactivateOneById(2, conn)
    |. andThenTest(
         "deactivateOneById",
         finish,
         fun
         | Some({id: 2, first_name: "patrick", active: 0, deleted: 0}) => success
         | x => toError(x),
       )
  );

  testAsync("deactivateOneById (succeeds but returns no result)", finish =>
    PersonModel2.deactivateOneById(1, conn)
    |. andThenTest(
         "deactivateOneById",
         finish,
         fun
         | None => success
         | x => toError(x),
       )
  );

  testAsync("deactivateOneById (fails and returns NotFound)", finish =>
    PersonModel.deactivateOneById(99, conn)
    |. shouldFail("deactivateOneById", finish)
  );

  testAsync("archiveOneById (returns 1 result)", finish =>
    PersonModel.archiveOneById(2, conn)
    |. andThenTest(
         "archiveOneById",
         finish,
         fun
         | Some({id: 2, first_name: "patrick"}) => success
         | x => toError(x),
       )
  );

  testAsync("archiveOneById (succeeds but returns no result)", finish =>
    PersonModel2.archiveOneById(3, conn)
    |. andThenTest(
         "archiveOneById",
         finish,
         fun
         | None => success
         | x => toError(x),
       )
  );

  testAsync("archiveOneById (fails and returns NotFound)", finish =>
    PersonModel.archiveOneById(99, conn)
    |. shouldFail("archiveOneById", finish)
  );

  testAsync("archiveCompoundBy (returns 1 result)", finish => {
    let where = b =>
      b |. SqlComposer.Select.where({j|AND $table.type_ = ?|j});
    let params = Json.Encode.([|string("dogfish")|]);
    AnimalModel.archiveCompoundBy(where, params, conn)
    |. andThenTest(
         "archiveCompoundBy",
         finish,
         fun
         | [|{id: 4, type_: "dogfish", deleted: 1}|] => success
         | x => toError(x),
       );
  });

  testAsync("archiveCompoundBy (succeeds but returns no results)", finish => {
    let where = b =>
      b |. SqlComposer.Select.where({j|AND $table.type_ = ?|j});
    let params = Json.Encode.([|string("moose")|]);
    AnimalModel2.archiveCompoundBy(where, params, conn)
    |. andThenTest(
         "archiveCompoundBy",
         finish,
         fun
         | [||] => success
         | x => toError(x),
       );
  });

  testAsync("archiveCompoundBy (fails and returns NotFound)", finish => {
    let where = b =>
      b |. SqlComposer.Select.where({j|AND $table.type_ = ?|j});
    let params = Json.Encode.([|string("blahblahblah")|]);
    AnimalModel.archiveCompoundBy(where, params, conn)
    |. shouldFail("archiveCompoundBy", finish);
  });

  testAsync("archiveCompoundBy (fails and returns EmptyUserQuery)", finish => {
    let where = b => b;
    let params = Json.Encode.([|string("blahblahblah")|]);
    AnimalModel.archiveCompoundBy(where, params, conn)
    |. shouldFail("archiveCompoundBy", finish);
  });

  testAsync("archiveCompoundOneById (returns 1 result)", finish =>
    AnimalModel.archiveCompoundOneById(2, conn)
    |. andThenTest(
         "archiveCompoundOneById",
         finish,
         fun
         | Some({id: 2, type_: "cat", deleted: 1}) => success
         | x => toError(x),
       )
  );

  testAsync("archiveCompoundOneById (succeeds but returns no result)", finish =>
    AnimalModel2.archiveCompoundOneById(3, conn)
    |. andThenTest(
         "archiveCompoundOneById",
         finish,
         fun
         | None => success
         | x => toError(x),
       )
  );

  testAsync("archiveCompoundOneById (fails and returns NotFound)", finish =>
    AnimalModel.archiveCompoundOneById(99, conn)
    |. shouldFail("archiveCompoundOneById", finish)
  );

  testAsync("deleteBy (returns 2 results)", finish => {
    let where = b =>
      b |. SqlComposer.Select.where({j|AND $table2.deleted != ?|j});
    let params = Json.Encode.([|int(0)|]);
    PersonModel.deleteBy(where, params, conn)
    |. andThenTest(
         "deleteBy",
         finish,
         fun
         | [|
             {id: 2, first_name: "patrick", age: 65, active: 0},
             {id: 3, first_name: "cody", age: 29, active: 1},
           |] => success
         | x => toError(x),
       );
  });

  testAsync("deleteBy (fails and returns NotFound)", finish => {
    let where = bs =>
      bs |. SqlComposer.Select.where({j|AND $table2.deleted != ?|j});
    let params = Json.Encode.([|int(0)|]);
    PersonModel.deleteBy(where, params, conn)
    |. shouldFail("deleteBy", finish);
  });

  testAsync("deleteOneById (returns 1 result)", finish =>
    AnimalModel.deleteOneById(3, conn)
    |. andThenTest(
         "deleteOneById",
         finish,
         fun
         | {id: 3, type_: "elephant", deleted: 1} => success
         | x => toError(x),
       )
  );

  testAsync("deleteOneById (fails and returns NotFound)", finish =>
    AnimalModel.deleteOneById(3, conn) |. shouldFail("deleteOneById", finish)
  );

  testAsync("incrementOneById (returns 1 result)", finish =>
    PersonModel.incrementOneById("age", 4, conn)
    |. andThenTest(
         "incrementOneById (returns 1 result)",
         finish,
         fun
         | Some({
             id: 4,
             first_name: "clinton",
             age: 41,
             active: 1,
             deleted: 0,
           }) => success
         | x => toError(x),
       )
  );

  testAsync("incrementOneById (succeeds but returns no result)", finish =>
    PersonModel2.incrementOneById("age", 5, conn)
    |. andThenTest(
         "incrementOneById",
         finish,
         fun
         | None => success
         | x => toError(x),
       )
  );

  testAsync("incrementOneById (fails and returns NotFound)", finish =>
    PersonModel.incrementOneById("age", 2, conn)
    |. shouldFail("incrementOneById (fails and returns NotFound)", finish)
  );

  testAsync("incrementOneById (fails and throws bad field error)", finish =>
    PersonModel.incrementOneById("badcolumn", 6, conn)
    |. shouldFail(
         "incrementOneById (fails and throw bad field error)",
         finish,
       )
  );
});
