let raw:
  (~db: Mysql.connection, ~sql: string, ~params: Js.Array.t('a)=?, 'b) =>
  Js.Promise.t((Mysql.results('c), Mysql.fields('d)));
