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
    PimpMySql_Query.getOneBy(sqlFactory(user), Config.decoder, params, conn);
  let get = (user, params, conn) =>
    PimpMySql_Query.get(sqlFactory(user), Config.decoder, params, conn);
  let getWhere = (user, params, conn) =>
    PimpMySql_Query.getWhere(
      sqlFactory(SqlComposer.Select.select),
      user,
      Config.decoder,
      params,
      conn,
    );
  let insertOne = (encoder, record, conn) =>
    PimpMySql_Query.insertOne(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      encoder,
      record,
      conn,
    );
  let insertBatch = (name, encoder, loader, error, columns, rows, conn) =>
    PimpMySql_Query.insertBatch(
      ~name,
      ~table=Config.table,
      ~encoder,
      ~loader,
      ~error,
      ~columns,
      ~rows,
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
  let deactivateOneById = (id, conn) =>
    PimpMySql_Query.deactivateOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      conn,
    );
  let archiveOneById = (id, conn) =>
    PimpMySql_Query.archiveOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      conn,
    );
  let archiveCompoundBy = (user, params, conn) =>
    PimpMySql_Query.archiveCompoundBy(
      sqlFactory(SqlComposer.Select.select),
      user,
      Config.table,
      Config.decoder,
      params,
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
  let deleteBy = (user, params, conn) =>
    PimpMySql_Query.deleteBy(
      sqlFactory(SqlComposer.Select.select),
      user,
      Config.table,
      Config.decoder,
      params,
      conn,
    );
  let deleteOneById = (id, conn) =>
    PimpMySql_Query.deleteOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      id,
      conn,
    );
  let incrementOneById = (field, id, conn) =>
    PimpMySql_Query.incrementOneById(
      sqlFactory(SqlComposer.Select.select),
      Config.table,
      Config.decoder,
      field,
      id,
      conn,
    );
};
