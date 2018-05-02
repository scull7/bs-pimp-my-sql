module type Config = {
  let table: string;
  let base: SqlComposer.Select.t;
};

module Generator: (Config: Config) => {
  let getById: (
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) => Js.Promise.t(option('a));
  let getByIdList: (
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
  let insert: (
    Js.Json.t => 'a,
    Json.Encode.encoder('b),
    'b,
    SqlCommon.Make_sql(MySql2).connection
  ) => Js.Promise.t(option('a));
  let update: (
    Js.Json.t => 'a,
    Json.Encode.encoder('b),
    'b,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) => Js.Promise.t(option('a));
  let softCompoundDelete: (
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) => Js.Promise.t([> `NotFound | `Ok(option('a)) ]);
};
