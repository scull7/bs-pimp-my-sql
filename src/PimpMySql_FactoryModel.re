module type Config = {let table: string; let base: SqlComposer.Select.t;};

module Generator = (Config: Config) => {
  let sqlFactory = PimpMySql_FactorySql.make(Config.table, Config.base);
  let getById = (decoder, id, conn) =>
    PimpMySql_Query.getById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      id,
      conn,
    );
  let getByIdList = (decoder, idList, conn) =>
    PimpMySql_Query.getByIdList(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      idList,
      conn,
    );
  let getOneBy = (user, decoder, params, conn) =>
    PimpMySql_Query.getOneBy(
      decoder,
      SqlComposer.Select.to_sql(sqlFactory(user)),
      params,
      conn,
    );
  let get = (user, decoder, params, conn) =>
    PimpMySql_Query.get(
      decoder,
      SqlComposer.Select.to_sql(sqlFactory(user)),
      params,
      conn,
    );
  let insert = (decoder, encoder, record, conn) =>
    PimpMySql_Query.insert(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      encoder,
      record,
      conn,
    );
  let updateById = (decoder, encoder, record, id, conn) =>
    PimpMySql_Query.updateById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      encoder,
      record,
      id,
      conn,
    );
  let softCompoundDeleteById = (decoder, id, conn) =>
    PimpMySql_Query.softCompoundDeleteById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      id,
      conn,
    );
};
