let getById: (
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
  Js.Json.t => 'a,
  string,
  Js.Json.t,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(option('a));

let get: (
  Js.Json.t => 'a,
  string,
  Js.Json.t,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t(array('a));

let insert: (
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
) => Js.Promise.t([> `Error('c) | `Ok(array('b)) ]);

let update: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  Json.Encode.encoder('b),
  'b,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t([> `NotFound | `Ok(option('a)) ]);

let softCompoundDelete: (
  SqlComposer.Select.t,
  string,
  Js.Json.t => 'a,
  int,
  SqlCommon.Make_sql(MySql2).connection
) => Js.Promise.t([> `NotFound | `Ok(option('a)) ]);
