module type Config = {
  type t;
  let table: string;
  let decoder: Js.Json.t => t;
  let base: SqlComposer.Select.t;
};

module Generator = (Config: Config) => {
  let sqlFactory = PimpMySql_FactorySql.make(Config.table, Config.base);

  let getOneById = id =>
    PimpMySql_Query.getOneById(sqlFactory, Config.table, Config.decoder, id);
  /**
    * @TODO - re-implement this when bs-sql-common supports batch
    *         query operations.
    */
  /*
   let getByIdList = idList =>
     PimpMySql_Query.getByIdList( sqlFactory(SqlComposer.Select.select),
       Config.table,
       Config.decoder,
       idList,
     );
   */

  let getOneBy = (user, params) =>
    PimpMySql_Query.getOneBy(user(sqlFactory), Config.decoder, params);

  let get = (user, params) =>
    PimpMySql_Query.get(user(sqlFactory), Config.decoder, params);

  let getWhere = (user, params) =>
    PimpMySql_Query.getWhere(sqlFactory, user, Config.decoder, params);

  let insertOne = (encoder, record) =>
    PimpMySql_Query.insertOne(
      sqlFactory,
      Config.table,
      Config.decoder,
      encoder,
      record,
    );

  let insertBatch = (name, encoder, loader, error, columns, rows) =>
    PimpMySql_Query.insertBatch(
      ~name,
      ~table=Config.table,
      ~encoder,
      ~loader,
      ~error,
      ~columns,
      ~rows,
    );

  let updateOneById = (encoder, record, id) =>
    PimpMySql_Query.updateOneById(
      sqlFactory,
      Config.table,
      Config.decoder,
      encoder,
      record,
      id,
    );

  let deactivateOneById = id =>
    PimpMySql_Query.deactivateOneById(
      sqlFactory,
      Config.table,
      Config.decoder,
      id,
    );

  let archiveOneById = id =>
    PimpMySql_Query.archiveOneById(
      sqlFactory,
      Config.table,
      Config.decoder,
      id,
    );

  let archiveCompoundBy = (user, params) =>
    PimpMySql_Query.archiveCompoundBy(
      sqlFactory,
      user,
      Config.table,
      Config.decoder,
      params,
    );

  let archiveCompoundOneById = id =>
    PimpMySql_Query.archiveCompoundOneById(
      sqlFactory,
      Config.table,
      Config.decoder,
      id,
    );

  let deleteBy = (user, params) =>
    PimpMySql_Query.deleteBy(sqlFactory, user, Config.decoder, params);

  let deleteOneById = id =>
    PimpMySql_Query.deleteOneById(
      sqlFactory,
      Config.table,
      Config.decoder,
      id,
    );

  let incrementOneById = (field, id) =>
    PimpMySql_Query.incrementOneById(
      sqlFactory,
      Config.table,
      Config.decoder,
      field,
      id,
    );
};
