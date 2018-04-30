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
};
