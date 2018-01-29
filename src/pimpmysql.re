let debug = Debug.make("PIMPMYSQL", "QUERY");

let logThenResolve = (db, sql, params) => {
  let sqlLabel = Chalk.blue("SQL:");
  let paramsLabel = Chalk.blue("PARAMS:");
  let paramsString = Util.inspect(Js.Option.getWithDefault([||], params));
  debug({j|$sqlLabel $sql \n$paramsLabel $paramsString|j});
  Js.Promise.resolve(db);
};

let raw = (~db, ~sql, ~params: option(Js.Array.t('a))=?, _) =>
  logThenResolve(db, sql, params)
  |> Mysql.Promise.pquery(~sql, ~placeholders=?params);
