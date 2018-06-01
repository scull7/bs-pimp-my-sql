/*
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
       Js.Promise.t(option(Config.t));
     let getByIdList:
       (list(int), SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(array(Config.t));
     let getOneBy:
       (
         SqlComposer.Select.t,
         Js.Json.t,
         SqlCommon.Make_sql(MySql2).connection
       ) =>
       Js.Promise.t(option(Config.t));
     let get:
       (
         SqlComposer.Select.t,
         Js.Json.t,
         SqlCommon.Make_sql(MySql2).connection
       ) =>
       Js.Promise.t(array(Config.t));
     let getWhere:
       (list(string), Js.Json.t, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(array(Config.t));
     let insertOne:
       (Json.Encode.encoder('b), 'b, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(option(Config.t));
     let updateOneById:
       (
         Json.Encode.encoder('b),
         'b,
         int,
         SqlCommon.Make_sql(MySql2).connection
       ) =>
       Js.Promise.t(Result.result(exn, option(Config.t)));
     let deactivateOneById:
       (int, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(Result.result(exn, option(Config.t)));
     let archiveOneById:
       (int, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(Result.result(exn, option(Config.t)));
     let archiveCompoundBy:
       (list(string), Js.Json.t, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(Result.result(exn, array(Config.t)));
     let archiveCompoundOneById:
       (int, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(Result.result(exn, option(Config.t)));
     let deleteBy:
       (list(string), Js.Json.t, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(Result.result(exn, array(Config.t)));
     let deleteOneById:
       (int, SqlCommon.Make_sql(MySql2).connection) =>
       Js.Promise.t(Result.result(exn, Config.t));
   };
   */
