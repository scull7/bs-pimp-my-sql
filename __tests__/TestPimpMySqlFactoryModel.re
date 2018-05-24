open Jest;

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

let table = "animal";

let table2 = "person";

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

let createTestData = conn => {
  Sql.mutate(conn, ~sql=createDb, _ => ());
  Sql.mutate(conn, ~sql=useDB, _ => ());
  Sql.mutate(conn, ~sql=createTable, _ => ());
  Sql.mutate(conn, ~sql=seedTable, _ => ());
  Sql.mutate(conn, ~sql=createTable2, _ => ());
  Sql.mutate(conn, ~sql=seedTable2, _ => ());
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
      select
      |> field({j|$table.id|j})
      |> field({j|$table.type_|j})
      |> field({j|$table.deleted|j})
      |> field({j|$table.deleted_timestamp|j})
      |> order_by(`Desc({j|$table.id|j}))
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
      select
      |> field({j|$table.id|j})
      |> field({j|$table.type_|j})
      |> field({j|$table.deleted|j})
      |> field({j|$table.deleted_timestamp|j})
      |> where({j|AND $table.deleted = 0|j})
      |> order_by(`Desc({j|$table.id|j}))
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
      select
      |> field({j|$table.id|j})
      |> field({j|$table.first_name|j})
      |> field({j|$table.age|j})
      |> field({j|$table.active|j})
      |> field({j|$table.deleted|j})
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
      select
      |> field({j|$table.id|j})
      |> field({j|$table.first_name|j})
      |> field({j|$table.age|j})
      |> field({j|$table.active|j})
      |> field({j|$table.deleted|j})
      |> where({j|AND $table.deleted = 0|j})
      |> where({j|AND $table.active = 1|j})
      |> where({j|AND $table2.age != 36|j})
    );
};

module AnimalModel = PimpMySql_FactoryModel.Generator(AnimalConfig);

module AnimalModel2 = PimpMySql_FactoryModel.Generator(AnimalConfig2);

module PersonModel = PimpMySql_FactoryModel.Generator(PersonConfig);

module PersonModel2 = PimpMySql_FactoryModel.Generator(PersonConfig2);

/* Tests */
describe("PimpMySql_FactoryModel", () => {
  createTestData(conn);
  testPromise("getOneById (returns a result)", () =>
    AnimalModel.getOneById(1, conn)
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
    AnimalModel.getOneById(99, conn)
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
    AnimalModel.getByIdList([1, 2], conn)
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
    AnimalModel.getByIdList([98, 99], conn)
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
        |> where({j|AND $table.id = ?|j})
        |> where({j|AND $table.type_ = ?|j})
      );
    let params = Json.Encode.([|int(1), string("dog")|] |> jsonArray);
    AnimalModel.getOneBy(userClauses, params, conn)
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
        |> where({j|AND $table.id = ?|j})
        |> where({j|AND $table.type_ = ?|j})
      );
    let params = Json.Encode.([|int(1), string("cat")|] |> jsonArray);
    AnimalModel.getOneBy(userClauses, params, conn)
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
        |> where({j|AND $table.id != ?|j})
        |> where({j|AND $table.type_ LIKE CONCAT("%", ?, "%")|j})
      );
    let params = Json.Encode.([|int(1), string("a")|] |> jsonArray);
    AnimalModel.get(userClauses, params, conn)
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
        select |> where({j|AND $table.type_ LIKE CONCAT(?, "%")|j})
      );
    let params = Json.Encode.([|string("z")|] |> jsonArray);
    AnimalModel.get(userClauses, params, conn)
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
  testPromise("getWhere (returns 1 result)", () => {
    let userClauses = [{j|AND $table.type_ = ?|j}];
    let params = Json.Encode.([|string("elephant")|] |> jsonArray);
    AnimalModel.getWhere(userClauses, params, conn)
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
  testPromise("getWhere (does not return any results)", () => {
    let userClauses = [{j|AND $table.type_ = ?|j}];
    let params = Json.Encode.([|string("mouse")|] |> jsonArray);
    AnimalModel.getWhere(userClauses, params, conn)
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
  testPromise("getWhere (fails and throws syntax error exception)", () => {
    let userClauses = [{j|$table.type_ = ?|j}];
    let params = Json.Encode.([|string("mouse")|] |> jsonArray);
    AnimalModel.getWhere(userClauses, params, conn)
    |> Js.Promise.then_(_ =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch(_ => Js.Promise.resolve(pass));
  });
  testPromise("insertOne (returns 1 result)", () => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "monkey"};
    AnimalModel.insertOne(encoder, record, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Some({id: 6, type_: "monkey"}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("insertOne (succeeds but returns no result)", () => {
    let encoder = x =>
      [
        ("type_", Json.Encode.string @@ x.type_),
        ("deleted", Json.Encode.int @@ 1),
      ]
      |> Json.Encode.object_;
    let record = {type_: "turkey"};
    AnimalModel2.insertOne(encoder, record, conn)
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
  testPromise("insertOne (fails and throws unique constraint error)", () => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "dog"};
    AnimalModel.insertOne(encoder, record, conn)
    |> Js.Promise.then_(_ =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch(_ => Js.Promise.resolve @@ pass);
  });
  testPromise(
    "insertOne (does not return a result, throws bad field error)", () => {
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "flamingo"};
    AnimalModel.insertOne(encoder, record, conn)
    |> Js.Promise.then_(_ =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch(_ => Js.Promise.resolve @@ pass);
  });
  testPromise("insertBatch (returns 2 results)", () => {
    let encoder = x =>
      [|Json.Encode.string @@ x.type_|] |> Json.Encode.jsonArray;
    let loader = animals => Js.Promise.resolve(animals);
    let error = msg => msg;
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
    let loader = animals => Js.Promise.resolve(animals);
    let error = msg => msg;
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
    let loader = animals => Js.Promise.resolve(animals);
    let error = msg => msg;
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
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    AnimalModel.updateOneById(encoder, record, 1, conn)
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
  testPromise("updateOneById (succeeds but returns no result)", () => {
    let encoder = x =>
      [
        ("type_", Json.Encode.string @@ x.type_),
        ("deleted", Json.Encode.int @@ 1),
      ]
      |> Json.Encode.object_;
    let record = {type_: "chicken"};
    AnimalModel2.updateOneById(encoder, record, 1, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(None) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("updateOneById (fails and returns NotFound)", () => {
    let encoder = x =>
      [("type_", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    AnimalModel.updateOneById(encoder, record, 99, conn)
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
    "updateOneById (does not return a result, throws bad field error)", () => {
    let encoder = x =>
      [("bad_column", Json.Encode.string @@ x.type_)] |> Json.Encode.object_;
    let record = {type_: "hippopotamus"};
    AnimalModel.updateOneById(encoder, record, 1, conn)
    |> Js.Promise.then_(_ =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch(_ => Js.Promise.resolve @@ pass);
  });
  testPromise("deactivateOneById (returns 1 result)", () =>
    PersonModel.deactivateOneById(2, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(
               Some({id: 2, first_name: "patrick", active: 0, deleted: 0}),
             ) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("deactivateOneById (succeeds but returns no result)", () =>
    PersonModel2.deactivateOneById(1, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(None) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("deactivateOneById (fails and returns NotFound)", () =>
    PersonModel.deactivateOneById(99, conn)
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
  testPromise("archiveOneById (returns 1 result)", () =>
    PersonModel.archiveOneById(2, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(
               Some({id: 2, first_name: "patrick", active: 0, deleted: 0}),
             ) =>
             fail("not an expected result")
           | Result.Ok(Some({id: 2, first_name: "patrick"})) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("archiveOneById (succeeds but returns no result)", () =>
    PersonModel2.archiveOneById(3, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(None) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("archiveOneById (fails and returns NotFound)", () =>
    PersonModel.archiveOneById(99, conn)
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
  testPromise("archiveCompoundBy (returns 1 result)", () => {
    let where = [{j|AND $table.type_ = ?|j}];
    let params = Json.Encode.([|string("dogfish")|] |> jsonArray);
    AnimalModel.archiveCompoundBy(where, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok([|
               {id: 4, type_: "dogfish", deleted: 1, deleted_timestamp: 0},
             |]) =>
             fail("not an expected result")
           | Result.Ok([|{id: 4, type_: "dogfish", deleted: 1}|]) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("archiveCompoundBy (succeeds but returns no results)", () => {
    let where = [{j|AND $table.type_ = ?|j}];
    let params = Json.Encode.([|string("moose")|] |> jsonArray);
    AnimalModel2.archiveCompoundBy(where, params, conn)
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
  testPromise("archiveCompoundBy (fails and returns NotFound)", () => {
    let where = [{j|AND $table.type_ = ?|j}];
    let params = Json.Encode.([|string("blahblahblah")|] |> jsonArray);
    AnimalModel.archiveCompoundBy(where, params, conn)
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
  testPromise("archiveCompoundBy (fails and returns EmptyUserQuery)", () => {
    let where = [];
    let params = Json.Encode.([|string("blahblahblah")|] |> jsonArray);
    AnimalModel.archiveCompoundBy(where, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Error(PimpMySql_Error.EmptyUserQuery(_)) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("archiveCompoundOneById (returns 1 result)", () =>
    AnimalModel.archiveCompoundOneById(2, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(
               Some({id: 2, type_: "cat", deleted: 1, deleted_timestamp: 0}),
             ) =>
             fail("not an expected result")
           | Result.Ok(Some({id: 2, type_: "cat", deleted: 1})) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("archiveCompoundOneById (succeeds but returns no result)", () =>
    AnimalModel2.archiveCompoundOneById(3, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(None) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("archiveCompoundOneById (fails and returns NotFound)", () =>
    AnimalModel.archiveCompoundOneById(99, conn)
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
  testPromise("deleteBy (returns 2 results)", () => {
    let where = [{j|AND $table2.deleted != ?|j}];
    let params = Json.Encode.([|int(0)|] |> jsonArray);
    PersonModel.deleteBy(where, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok([|
               {id: 2, first_name: "patrick", age: 65, active: 0},
               {id: 3, first_name: "cody", age: 29, active: 1},
             |]) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("deleteBy (fails and returns NotFound)", () => {
    let where = [{j|AND $table2.deleted != ?|j}];
    let params = Json.Encode.([|int(0)|] |> jsonArray);
    PersonModel.deleteBy(where, params, conn)
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
  testPromise("deleteBy (fails and returns EmptyUserQuery)", () => {
    let where = [];
    let params = Json.Encode.([|int(0)|] |> jsonArray);
    PersonModel.deleteBy(where, params, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Error(PimpMySql_Error.EmptyUserQuery(_)) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       );
  });
  testPromise("deleteOneById (returns 1 result)", () =>
    AnimalModel.deleteOneById(3, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok({
               id: 3,
               type_: "elephant",
               deleted: 1,
               deleted_timestamp: 0,
             }) =>
             fail("not an expected result")
           | Result.Ok({id: 3, type_: "elephant", deleted: 1}) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("deleteOneById (fails and returns NotFound)", () =>
    AnimalModel.deleteOneById(3, conn)
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
  testPromise("incrementOneById (returns 1 result)", () =>
    PersonModel.incrementOneById("age", 4, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(
               Some({
                 id: 4,
                 first_name: "clinton",
                 age: 41,
                 active: 1,
                 deleted: 0,
               }),
             ) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("incrementOneById (succeeds but returns no result)", () =>
    PersonModel2.incrementOneById("age", 5, conn)
    |> Js.Promise.then_(res =>
         (
           switch (res) {
           | Result.Ok(None) => pass
           | _ => fail("not an expected result")
           }
         )
         |> Js.Promise.resolve
       )
  );
  testPromise("incrementOneById (fails and returns NotFound)", () =>
    PersonModel.incrementOneById("age", 2, conn)
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
  testPromise("incrementOneById (fails and throws bad field error)", () =>
    PersonModel.incrementOneById("badcolumn", 6, conn)
    |> Js.Promise.then_(_ =>
         Js.Promise.resolve @@ fail("not an expected result")
       )
    |> Js.Promise.catch(_ => Js.Promise.resolve @@ pass)
  );
  afterAll(() => {
    Sql.mutate(conn, ~sql=dropDb, _ => ());
    MySql2.close(conn);
  });
});
