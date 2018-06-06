type t;

let make: (string, SqlComposer.Select.t) => SqlComposer.Select.t;

let addField: (t, string) => t;

let groupBy: (t, string) => t;

let orderBy: (t, [ | `Asc(string) | `Desc(string)]) => t;

let andWhere: (t, string) => t;

let orWhere: (t, string) => t;

let toSql: t => string;
