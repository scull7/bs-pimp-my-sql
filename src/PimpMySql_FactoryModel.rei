module type Config = {
  type t;
  let connection: SqlCommon.Make_sql(MySql2).connection;
  let table: string;
  let decoder: Js.Json.t => t;
  let base: SqlComposer.Select.t;
};

module Generator: (Config: Config) => {
  let getOneById: (
    int,
  ) => Js.Promise.t(option(Config.t));
  let getByIdList: (
    list(int),
  ) => Js.Promise.t(array(Config.t));
  let getOneBy: (
    SqlComposer.Select.t,
    Js.Json.t,
  ) => Js.Promise.t(option(Config.t));
  let get: (
    SqlComposer.Select.t,
    Js.Json.t,
  ) => Js.Promise.t(array(Config.t));
  let getWhere: (
    list(string),
    Js.Json.t,
  ) => Js.Promise.t(array(Config.t));
  let insertOne: (
    Json.Encode.encoder('b),
    'b,
  ) => Js.Promise.t(option(Config.t));
  let updateOneById: (
    Json.Encode.encoder('b),
    'b,
    int,
  ) => Js.Promise.t(Result.result(exn, option(Config.t)));
  let archiveCompoundOneById: (
    int,
  ) => Js.Promise.t(Result.result(exn, option(Config.t)));
  let deleteOneById: (
    int,
  ) => Js.Promise.t(Result.result(exn, Config.t));
};
