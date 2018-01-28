![Project Status](https://img.shields.io/badge/status-alpha-red.svg)

# bs-pimp-my-sql
ReasonML implementation of the [pimp-my-sql] wrapper.

## Why?

This is a SQL wrapper that provides many convenience features and a "light" ORM interface that
uses plain SQL as the underlying language.  Currently it is only compatible with the [bs-mysql2]
client.

## How do I install it?

Inside of a BuckleScript project:
```shell
yarn add bs-mysql2 bs-pimp-my-sql
```

Then add `bs-mysql2` and `bs-pimp-my-sql` to your `bs-dependencies`
in `bsconfig.json`:

```json
{
  "bs-dependencies": [
    "bs-mysql2",
    "bs-pimp-my-sql"
  ]
}
```

## How do I use it?

### Execute a raw SQL query.
```reason
let db =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Pimpmysql.raw(~db, ~sql="SELECT ? + ? AS result", ~params=[|5, 6|], ())
|> Js.Promise.then_(res => {
     Js.log(res);
     Js.Promise.resolve(1);
   })
|> Mysql.Promise.Connection.end_(db)
|> Js.Promise.catch(err => {
     Js.log(err);
     Mysql.Connection.end_(db);
     Js.Promise.resolve(-1);
   });
```

### Run the examples
* `yarn run examples:basic`

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
