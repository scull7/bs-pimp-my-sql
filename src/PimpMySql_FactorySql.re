open SqlComposer;
type t = Select.t;

/* Public */
let make = (table, base) => Select.(base |. from({j|`$table`|j}));
