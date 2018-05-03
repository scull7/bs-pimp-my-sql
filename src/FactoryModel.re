module type Config = {let table: string; let base: SqlComposer.Select.t;};

module Generator = (Config: Config) => {
  let sqlFactory = FactorySql.make(Config.table, Config.base);
  let getById = (decoder, id, conn) =>
    Query.getById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      id,
      conn,
    );
  let getByIdList = (decoder, idList, conn) =>
    Query.getByIdList(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      idList,
      conn,
    );
  let getOneBy = (user, decoder, params, conn) =>
    Query.getOneBy(
      decoder,
      SqlComposer.Select.to_sql(sqlFactory(user)),
      params,
      conn,
    );
  let get = (user, decoder, params, conn) =>
    Query.get(
      decoder,
      SqlComposer.Select.to_sql(sqlFactory(user)),
      params,
      conn,
    );
  let insert = (decoder, encoder, record, conn) =>
    Query.insert(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      encoder,
      record,
      conn,
    );
  let update = (decoder, encoder, record, id, conn) =>
    Query.update(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      encoder,
      record,
      id,
      conn,
    );
  let softCompoundDeleteById = (decoder, id, conn) =>
    Query.softCompoundDeleteById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      decoder,
      id,
      conn,
    );
};
