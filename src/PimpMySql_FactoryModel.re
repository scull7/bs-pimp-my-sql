module type Config = {
  type t;
  let table: string;
  let decoder: Js.Json.t => t;
  let base: SqlComposer.Select.t;
};

module Generator = (Config: Config) => {
  let sqlFactory = PimpMySql_FactorySql.make(Config.table, Config.base);
  let getOneById = (id, conn) =>
    PimpMySql_Query.getOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      conn,
    );
  let getByIdList = (idList, conn) =>
    PimpMySql_Query.getByIdList(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      idList,
      conn,
    );
  let getOneBy = (user, params, conn) =>
    PimpMySql_Query.getOneBy(
      Config.decoder,
      SqlComposer.Select.to_sql(sqlFactory(user)),
      params,
      conn,
    );
  let get = (user, params, conn) =>
    PimpMySql_Query.get(
      Config.decoder,
      SqlComposer.Select.to_sql(sqlFactory(user)),
      params,
      conn,
    );
  let insert = (encoder, record, conn) =>
    PimpMySql_Query.insert(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      encoder,
      record,
      conn,
    );
  let updateOneById = (encoder, record, id, conn) =>
    PimpMySql_Query.updateOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      encoder,
      record,
      id,
      conn,
    );
  let archiveCompoundOneById = (id, conn) =>
    PimpMySql_Query.archiveCompoundOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      conn,
    );
};
