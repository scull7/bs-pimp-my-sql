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
      (SqlCommon.Make(MySql2).Id.t, SqlCommon.Make(MySql2).Connection.t) =>
      Future.t(Belt.Result.t(option(Config.t), exn));
    /**
      * @TODO - bs-sql-common query_batch method needs to be implemented before
      *         this will work.
      */
    /*
     let getByIdList:
       (list(int), SqlCommon.Make(MySql2).Connection.t) =>
       Future.t(Belt.Result.t(array(Config.t), exn));
     */

    let getOneBy:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let get:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let getWhere:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let insertOne:
      (Json.Encode.encoder('b), 'b, SqlCommon.Make(MySql2).Connection.t) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let insertBatch:
      (
        string,
        'a => array(Js.Json.t),
        array('a) => Future.t(Belt.Result.t(array('b), exn)),
        string => exn,
        array(string),
        array('a),
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(array('b), exn));

    let updateOneById:
      (
        Json.Encode.encoder('b),
        'b,
        SqlCommon.Make(MySql2).Id.t,
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let deactivateOneById:
      (SqlCommon.Make(MySql2).Id.t, SqlCommon.Make(MySql2).Connection.t) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let archiveOneById:
      (SqlCommon.Make(MySql2).Id.t, SqlCommon.Make(MySql2).Connection.t) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let archiveCompoundBy:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let archiveCompoundOneById:
      (SqlCommon.Make(MySql2).Id.t, SqlCommon.Make(MySql2).Connection.t) =>
      Future.t(Belt.Result.t(option(Config.t), exn));

    let deleteBy:
      (
        SqlComposer.Select.t => SqlComposer.Select.t,
        array(Js.Json.t),
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(array(Config.t), exn));

    let deleteOneById:
      (SqlCommon.Make(MySql2).Id.t, SqlCommon.Make(MySql2).Connection.t) =>
      Future.t(Belt.Result.t(Config.t, exn));

    let incrementOneById:
      (
        string,
        SqlCommon.Make(MySql2).Id.t,
        SqlCommon.Make(MySql2).Connection.t
      ) =>
      Future.t(Belt.Result.t(option(Config.t), exn));
  };
