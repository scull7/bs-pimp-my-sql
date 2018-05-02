[![NPM](https://nodei.co/npm/bs-pimp-my-sql.png)](https://nodei.co/npm/bs-pimp-my-sql/)
[![Build Status](https://www.travis-ci.org/scull7/bs-pimp-my-sql.svg?branch=master)](https://www.travis-ci.org/scull7/bs-pimp-my-sql)
[![Coverage Status](https://coveralls.io/repos/github/scull7/bs-pimp-my-sql/badge.svg?branch=master)](https://coveralls.io/github/scull7/bs-pimp-my-sql?branch=master)

# bs-pimp-my-sql
ReasonML implementation of the [pimp-my-sql] wrapper.

## Why?

This is a SQL wrapper that provides many convenience features and a "light" ORM interface that
uses plain SQL as the underlying language.  Currently it is only compatible with the [bs-mysql2]
client.

## How do I install it?

Inside of a BuckleScript project:
```shell
yarn add bs-pimp-my-sql
```

## How do I use it?

### Using the Factory Model.
```reason
module Sql = SqlCommon.Make_sql(MySql2);

let conn = Sql.connect(~host="127.0.0.1", ~port=3306, ~user="root", ~database="example", ());

let table = "animal";

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

module Model = FactoryModel.Generator(Config);

let decoder = json =>
  Json.Decode.{
    id: field("id", int, json),
    type_: field("type_", string, json),
    deleted: field("deleted", int, json),
  };

Model.getById(decoder, 1, conn)
|> Js.Promise.then_(res =>
     (
       switch (res) {
       | Some(x) => <handle case for result>
       | None => <handle case for no result>
       }
     )
     |> Js.Promise.resolve
   );

## What's missing?

Everything not checked...

- [ ] Query Interface
  - [x] _(raw)_ Raw SQL query
  - [ ] _(rawInsert)_ Raw SQL insert
  - [ ] _(rawUpdate)_ Raw SQL update
  - [ ] INSERT
    - [ ] _(insert)_ basic wrapper
  - [ ] UPDATE
    - [ ] _(update)_ Basic wrapper
    - [ ] _(updateWhereParams)_ with the `ObjectHash` interface
    - [ ] _(increment)_ increment an integer column - must fit the `Counter` interface
  - [ ] DELETE
    - [ ] _(delete)_ using a custom where clause
    - [ ] _(deleteById)_ - must fit the `PrimaryId` interface
  - [ ] Archive
    - [ ] _(deactivate)_ Deactivate a row - must fit the `Activated` interface
    - [ ] _(archive)_ Soft DELETE a row - must fit the `Archive` interface
    - [ ] _(archiveCompound)_ Soft Compound DELETE a row - must fit the `ArchiveCompound` interface
  - [ ] SELECT
    - [ ] Transforms
      - [ ] JSON column parse
      - [ ] Nest dot notation transform
      - [ ] Null out nested objects
    - [ ] _(get)_ using the Compositional SQL DSL
    - [ ] _getWhere)_ using a custom where clause
    - [ ] _(getOne)_ with custom where clause
    - [ ] _(getOneById)_ using the `id` column - must fit `PrimaryId` interface
    - [ ] _(getWhereParams)_ using the `ObjectHash` interface
- [ ] Model
  - [ ] Compositional SQL DSL
  - [ ] Model Creation DSL
  - [ ] Query interface
    - [ ] INSERT
      - [ ] _(create)_
      - [ ] Pre-Create intercept
      - [ ] Post-Create intercept
    - [ ] UPDATE
      - [ ] _(update)_
      - [ ] Pre-Update intercept
      - [ ] Post-Update intercept
      - [ ] _(increment)_ increment an integer column - must fit the `Counter` interface
    - [ ] DELETE
      - [ ] _(delete)_ using a custom where clause
      - [ ] _(deleteById)_ - must fit the `PrimaryId` interface
    - [ ] Archive
      - [ ] _(deactivate)_ Deactivate a row - must fit the `Activated` interface
      - [ ] _(archive)_ Soft DELETE a row - must fit the `Archive` interface
      - [ ] _(archiveCompound)_ Soft Compound DELETE a row - must fit the `ArchiveCompound` interface
    - [ ] SELECT
      - [ ] Transforms - _(Dependent upon Query Interface implementation)_
      - [ ] _(get)_ using the Compositional SQL DSL
      - [ ] _(getOne)_ with custom where clause
      - [ ] _(getOneById)_ using the `id` column - must fit `PrimaryId` interface
      - [ ] _(getWhere)_ using a custom where clause
      - [ ] _(getWhereParams)_ using the `ObjectHash` interface
- [ ] Search - _This needs some re-design to better fit ReasonML language semantics._
- [ ] Utilities
  - [ ] TIMESTAMP conversion
  - [ ] `ObjectHash` interface interpolation
  - [ ] Caching

[pimp-my-sql]: https://github.com/influentialpublishers/pimp-my-sql
[bs-mysql]: https://github.com/davidgomes/bs-mysql
[mysql2]: https://www.npmjs.com/package/mysql2
