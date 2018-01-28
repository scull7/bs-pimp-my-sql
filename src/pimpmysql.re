let raw = (~db, ~sql, ~params=?, _) =>
  Js.Promise.resolve(db) |> Mysql.Promise.pquery(~sql, ~placeholders=?params);
