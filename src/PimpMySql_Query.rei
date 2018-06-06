let getOneById:
  (
    SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(option('a), exn));

/**
 * @TODO - bs-sql-common query_batch method needs to be implemented before
 *         this will work.
 */
/*
 let getByIdList:
   (
     SqlComposer.Select.t,
     string,
     Js.Json.t => 'a,
     list(int),
     SqlCommon.Make_sql(MySql2).connection
   ) =>
   Future.t(Belt.Result.t(array('a), exn));
 */

let getOneBy:
  (
    SqlComposer.Select.t,
    Js.Json.t => 'a,
    array(Js.Json.t),
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(option('a), exn));

let get:
  (
    SqlComposer.Select.t,
    Js.Json.t => 'a,
    array(Js.Json.t),
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(array('a), exn));

let getWhere:
  (
    SqlComposer.Select.t,
    SqlComposer.Select.t => SqlComposer.Select.t,
    Js.Json.t => 'a,
    array(Js.Json.t),
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(array('a), exn));

let insertOne:
  (
    SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    Json.Encode.encoder('b),
    'b,
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(option('a), exn));

let insertBatch:
  (
    ~name: string,
    ~table: string,
    ~encoder: 'a => Js.Json.t,
    ~loader: array('a) => Future.t(Belt.Result.t(array('b), exn)),
    ~error: string => exn,
    ~columns: array(string),
    ~rows: array('a),
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(array('b), exn));

let updateOneById:
  (
    SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    Json.Encode.encoder('b),
    'b,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(option('a), exn));

let deactivateOneById:
  (
    SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(option('a), exn));

let archiveOneById:
  (
    SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(option('a), exn));

let archiveCompoundBy:
  (
    SqlComposer.Select.t,
    SqlComposer.Select.t => SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    array(Js.Json.t),
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(array('a), exn));

let archiveCompoundOneById:
  (
    SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(option('a), exn));

let deleteBy:
  (
    SqlComposer.Select.t,
    SqlComposer.Select.t => SqlComposer.Select.t,
    Js.Json.t => 'a,
    array(Js.Json.t),
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t(array('a), exn));

let deleteOneById:
  (
    SqlComposer.Select.t,
    string,
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) =>
  Future.t(Belt.Result.t('a, exn));
