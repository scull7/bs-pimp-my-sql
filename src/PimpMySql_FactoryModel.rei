module type Config = {
  type t;
  let table: string;
  let decoder: Js.Json.t => t;
  let base: SqlComposer.Select.t;
};

module Generator:
  (Config: Config) =>
  {
    let getOneById:
      (int, SqlCommon.Make_sql(MySql2).connection) =>
      Future.t(Belt.Result.t(option(Config.t), exn));
    /**
      * @TODO - bs-sql-common query_batch method needs to be implemented before
      *         this will work.
      */
    /*
     let getByIdList:
       (list(int), SqlCommon.Make_sql(MySql2).connection) =>
       Future.t(Belt.Result.t(array(Config.t), exn));
     */

    let getOneBy:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make_sql(MySql2).connection
      ) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let get:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make_sql(MySql2).connection
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let getWhere:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make_sql(MySql2).connection
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let insertOne:
      (Json.Encode.encoder('b), 'b, SqlCommon.Make_sql(MySql2).connection) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let updateOneById:
      (
        Json.Encode.encoder('b),
        'b,
        int,
        SqlCommon.Make_sql(MySql2).connection
      ) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let deactivateOneById:
      (int, SqlCommon.Make_sql(MySql2).connection) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let archiveOneById:
      (int, SqlCommon.Make_sql(MySql2).connection) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let archiveCompoundBy:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make_sql(MySql2).connection
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let archiveCompoundOneById:
      (int, SqlCommon.Make_sql(MySql2).connection) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let deleteBy:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make_sql(MySql2).connection
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let deleteOneById:
      (int, SqlCommon.Make_sql(MySql2).connection) =>
      Future.t(Belt.Result.t(Config.t, exn));
  };
