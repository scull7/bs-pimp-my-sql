module type Config = {
  type t;
  let connection: SqlCommon.Make_sql(MySql2).connection;
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
      Config.connection,
    );
  let getByIdList = idList =>
    PimpMySql_Query.getByIdList(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      idList,
      Config.connection,
    );
  let getOneBy = (user, params) =>
    PimpMySql_Query.getOneBy(
      sqlFactory(user),
      Config.decoder,
      params,
      Config.connection,
    );
  let get = (user, params) =>
    PimpMySql_Query.get(
      sqlFactory(user),
      Config.decoder,
      params,
      Config.connection,
    );
  let insertOne = (encoder, record) =>
    PimpMySql_Query.insertOne(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      encoder,
      record,
      Config.connection,
    );
  let updateOneById = (encoder, record, id) =>
    PimpMySql_Query.updateOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      encoder,
      record,
      id,
      Config.connection,
    );
  let archiveCompoundOneById = id =>
    PimpMySql_Query.archiveCompoundOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      Config.connection,
    );
};
