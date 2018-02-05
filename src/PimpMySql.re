let debug = Debug.make("PIMPMYSQL", "QUERY");

module Promise = SqlCommon.Promise;

module Response = SqlCommon.Response;

let logThenResolve = (db, sql, params) => {
  let sqlLabel = Chalk.blue("SQL:");
  let paramsLabel = Chalk.blue("PARAMS:");
  let paramsString = Util.inspect(Js.Option.getWithDefault([||], params));
  debug({j|$sqlLabel $sql \n$paramsLabel $paramsString|j});
  Js.Promise.resolve(db);
};

let raw = (~db, ~sql, ~params=?, _) =>
  logThenResolve(db, sql, params)
  |> Js.Promise.then_(conn =>
       switch params {
       | Some(p) => Promise.with_params(conn, sql, p)
       | None => Promise.raw(conn, sql)
       }
     );
