let getOneById: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(option('a));

let getByIdList: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  list(int),
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(array('a));
  
let getOneBy: (
  SqlComposer.Select.t,
  Js.Json.t => 'a,
  Js.Json.t,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(option('a));

let get: (
  SqlComposer.Select.t,
  Js.Json.t => 'a,
  Js.Json.t,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(array('a));

let getWhere: (
  SqlComposer.Select.t,
  list(string),
  Js.Json.t => 'a,
  Js.Json.t,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(array('a));

let insertOne: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  Json.Encode.encoder('b),
  'b,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(option('a));

let insertBatch: (
  ~name: string,
  ~table: string,
  ~encoder: 'a => Js.Json.t,
  ~loader: array('a) => Js.Promise.t(array('b)),
  ~error: string => 'c,
  ~columns: array(string),
  ~rows: array('a),
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result('c, array('b)));

let updateOneById: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  Json.Encode.encoder('b),
  'b,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result(exn, option('a)));

let deactivateOneById: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result(exn, option('a)));

let archiveOneById: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result(exn, option('a)));

let archiveCompoundBy: (
  SqlComposer.Select.t,
  list(string),
  string,
  Js.Json.t => 'a,
  Js.Json.t,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result(exn, array('a)));

let archiveCompoundOneById: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result(exn, option('a)));

let deleteBy: (
  SqlComposer.Select.t,
  list(string),
  string,
  Js.Json.t => 'a,
  Js.Json.t,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result(exn, array('a)));

let deleteOneById: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(Result.result(exn, 'a));
