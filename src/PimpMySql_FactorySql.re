open SqlComposer;
type t = Select.t;

/* Public */
let make = (table, base) => Select.(base |. from({j| FROM $table |j}));

let addField = (t, field) => Select.field(t, field);

let groupBy = (t, clause) => Select.groupBy(t, clause);

let orderBy = (t, clause) => Select.orderBy(t, clause);

let andWhere = (t, clause) => Select.where(t, {j|AND $clause |j});

let orWhere = (t, clause) => Select.where(t, {j| OR $clause |j});

let toSql = t => Select.toSql(t);
