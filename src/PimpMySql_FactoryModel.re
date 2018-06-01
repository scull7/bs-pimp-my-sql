/*
 module type Config = {
   type t;
   let table: string;
   let decoder: Js.Json.t => t;
   let base: SqlComposer.Select.t;
 };

 module Generator = (Config: Config) => {
   let sqlFactory = PimpMySql_FactorySql.make(Config.table, Config.base);
   let getOneById = id =>
     PimpMySql_Query.getOneById(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       id,
     );
   let getByIdList = idList =>
     PimpMySql_Query.getByIdList(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       idList,
     );
   let getOneBy = (user, params) =>
     PimpMySql_Query.getOneBy(sqlFactory(user), Config.decoder, params);
   let get = (user, params) =>
     PimpMySql_Query.get(sqlFactory(user), Config.decoder, params);
   let getWhere = (user, params) =>
     PimpMySql_Query.getWhere(
       sqlFactory(SqlComposer.Select.select),
       user,
       Config.decoder,
       params,
     );
   let insertOne = (encoder, record) =>
     PimpMySql_Query.insertOne(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       encoder,
       record,
     );
   let updateOneById = (encoder, record, id) =>
     PimpMySql_Query.updateOneById(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       encoder,
       record,
       id,
     );
   let deactivateOneById = id =>
     PimpMySql_Query.deactivateOneById(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       id,
     );
   let archiveOneById = id =>
     PimpMySql_Query.archiveOneById(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       id,
     );
   let archiveCompoundBy = (user, params) =>
     PimpMySql_Query.archiveCompoundBy(
       sqlFactory(SqlComposer.Select.select),
       user,
       Config.table,
       Config.decoder,
       params,
     );
   let archiveCompoundOneById = id =>
     PimpMySql_Query.archiveCompoundOneById(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       id,
     );
   let deleteBy = (user, params) =>
     PimpMySql_Query.deleteBy(
       sqlFactory(SqlComposer.Select.select),
       user,
       Config.table,
       Config.decoder,
       params,
     );
   let deleteOneById = id =>
     PimpMySql_Query.deleteOneById(
       sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       id,
     );
 };
 */
