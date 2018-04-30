module type Config = {
  let table: string;
  let base: SqlComposer.Select.t;
};

module Generator: (Config: Config) => {
  let getById: (
    Js.Json.t => 'a,
    int,
    SqlCommon.Make_sql(MySql2).connection
  ) => Js.Promise.t(option('a));
};
