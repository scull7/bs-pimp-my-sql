let named: Js.Json.t => option(MySql2.Params.t);

let positional: array(Js.Json.t) => option(MySql2.Params.t);
