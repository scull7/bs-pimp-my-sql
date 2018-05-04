module type Config = {
  type t;
  let table: string;
  let decoder: Js.Json.t => t;
  let base: SqlComposer.Select.t;
};

module Generator = (Config: Config) => {
  let sqlFactory = PimpMySql_FactorySql.make(Config.table, Config.base);
  let getOneById = (id, connection) =>
    PimpMySql_Query.getOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      connection,
    );
  let getByIdList = (idList, connection) =>
    PimpMySql_Query.getByIdList(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      idList,
      connection,
    );
  let getOneBy = (user, params, connection) =>
    PimpMySql_Query.getOneBy(
      sqlFactory(user),
      Config.decoder,
      params,
      connection,
    );
  let get = (user, params, connection) =>
    PimpMySql_Query.get(
      sqlFactory(user),
      Config.decoder,
      params,
      connection,
    );
  let insertOne = (encoder, record, connection) =>
    PimpMySql_Query.insertOne(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      encoder,
      record,
      connection,
    );
  let updateOneById = (encoder, record, id, connection) =>
    PimpMySql_Query.updateOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      encoder,
      record,
      id,
      connection,
    );
  let archiveCompoundOneById = (id, connection) =>
    PimpMySql_Query.archiveCompoundOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      connection,
    );
};
