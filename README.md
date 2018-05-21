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
yarn add @glennsl/bs-json bs-mysql2 bs-pimp-my-sql bs-result bs-sql-common bs-sql-composer
```

Then add `@glennsl/bs-json`, `bs-mysql2`, `bs-pimp-my-sql`, `bs-result`, `bs-sql-common`,
and `bs-sql-composer` to your `bs-dependencies` in `bsconfig.json`:

```json
{
  "bs-dependencies": [
    "@glennsl/bs-json",
    "bs-mysql2",
    "bs-pimp-my-sql",
    "bs-result",
    "bs-sql-common",
    "bs-sql-composer"
  ]
}
```

## How do I use it?

### General Usage

The way you should access modules in `PimpMySql` is as follows:

```reason
PimpMySql.<Module>
```

Of course you can always directly call the internal files, namespaced with `PimpMySql_`, but
that is not recommended since these files are implementation details.

### Using the Factory Model.
```reason
module Sql = SqlCommon.Make_sql(MySql2);

let conn = Sql.connect(~host="127.0.0.1", ~port=3306, ~user="root", ~database="example", ());

let table = "animal";

type animal = {
  id: int,
  type_: string,
  deleted: int,
};

module Config = {
  type t = animal;
  let table = table;
  let decoder = json =>
    Json.Decode.{
      id: field("id", int, json),
      type_: field("type_", string, json),
      deleted: field("deleted", int, json),
    };
  let base =
    SqlComposer.Select.(
      select
      |> field({j|$table.`id`|j})
      |> field({j|$table.`type_`|j})
      |> field({j|$table.`deleted`|j})
      |> order_by(`Desc({j|$table.`id`|j}))
    );
};

module Model = PimpMySql.FactoryModel.Generator(Config);

Model.getOneById(1, conn)
|> Js.Promise.then_(res =>
     (
       switch (res) {
       | Some(x) => <handle case for result>
       | None => <handle case for no result>
       }
     )
     |> Js.Promise.resolve
   );
```

## What's missing?

Everything not checked...

- [ ] Query Interface
  - [x] _(raw)_ Raw SQL query
  - [ ] _(rawInsert)_ Raw SQL insert
  - [ ] _(rawUpdate)_ Raw SQL update
  - [x] INSERT
    - [x] _(insertOne)_ basic wrapper
    - [x] _(insertBatch)_ basic wrapper
  - [ ] UPDATE
    - [x] _(updateOneById)_ Basic wrapper using the `id` column - must fit `PrimaryId` interface
    - [ ] _(updateWhereParams)_ with the `ObjectHash` interface
    - [x] _(incrementOneById)_ increment an integer column using the `id` column - must fit the `Counter`
          and `PrimaryId` interfaces
  - [x] DELETE
    - [x] _(deleteBy)_ using a custom where clause
    - [x] _(deleteOneById)_ - must fit the `PrimaryId` interface
  - [x] Archive
    - [x] _(deactivateOneById)_ Deactivate a row using the `id` column - must fit the `Activated` and
          `PrimaryId` interfaces
    - [x] _(archiveOneById)_ Soft DELETE a row using the `id` column - must fit the `Archive` interface
    - [x] _(archiveCompoundBy)_ Soft Compound DELETE using a custom where clause - must fit the
          `ArchiveCompound` interface
    - [x] _(archiveCompoundOneById)_ Soft Compound DELETE a row using the `id` column - must fit the
          `ArchiveCompound` and `PrimaryId` interfaces
  - [ ] SELECT
    - [ ] Transforms
      - [ ] JSON column parse
      - [ ] Nest dot notation transform
      - [ ] Null out nested objects
    - [x] _(get)_ using the Compositional SQL DSL
    - [x] _(getByIdList)_ using the `id` column - must fit `PrimaryId` interface
    - [x] _(getOneBy)_ with custom where clause
    - [x] _(getOneById)_ using the `id` column - must fit `PrimaryId` interface
    - [x] _(getWhere)_ using a custom where clause
    - [ ] _(getWhereParams)_ using the `ObjectHash` interface
- [ ] Model
  - [x] Compositional SQL DSL
  - [x] Model Creation DSL
  - [x] Query interface
    - [ ] INSERT
      - [x] _(insertOne)_
      - [x] _(insertBatch)_
      - [ ] Pre-Create intercept
      - [ ] Post-Create intercept
    - [ ] UPDATE
      - [x] _(updateOneById)_ using the `id` column - must fit `PrimaryId` interface
      - [x] _(incrementOneById)_ increment an integer column using the `id` column - must fit the `Counter`
            and `PrimaryId` interfaces
      - [ ] Pre-Update intercept
      - [ ] Post-Update intercept
    - [x] DELETE
      - [x] _(deleteBy)_ using a custom where clause
      - [x] _(deleteOneById)_ - must fit the `PrimaryId` interface
    - [x] Archive
      - [x] _(deactivateOneById)_ Deactivate a row using the `id` column - must fit the `Activated` and
            `PrimaryId` interfaces
      - [x] _(archiveOneById)_ Soft DELETE a row using the `id` column - must fit the `Archive` interface
      - [x] _(archiveCompoundBy)_ Soft Compound DELETE using a custom where clause - must fit the
            `ArchiveCompound` interface
      - [x] _(archiveCompoundOneById)_ Soft Compound DELETE a row using the `id` column - must fit the
            `ArchiveCompound` and `PrimaryId` interfaces
    - [ ] SELECT
      - [ ] Transforms - _(Dependent upon Query Interface implementation)_
      - [x] _(get)_ using the Compositional SQL DSL
      - [x] _(getByIdList)_ using the `id` column - must fit `PrimaryId` interface
      - [x] _(getOneBy)_ with custom where clause
      - [x] _(getOneById)_ using the `id` column - must fit `PrimaryId` interface
      - [x] _(getWhere)_ using a custom where clause
      - [ ] _(getWhereParams)_ using the `ObjectHash` interface
- [ ] Search - _This needs some re-design to better fit ReasonML language semantics._
- [ ] Utilities
  - [ ] TIMESTAMP conversion
  - [ ] `ObjectHash` interface interpolation
  - [ ] Caching

[pimp-my-sql]: https://github.com/influentialpublishers/pimp-my-sql
[bs-mysql]: https://github.com/davidgomes/bs-mysql
[mysql2]: https://www.npmjs.com/package/mysql2
