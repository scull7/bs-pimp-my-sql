/* Private */
let mergeGroupBy = (base, userClauses) =>
  SqlComposer.Select.(
    if (List.length(userClauses) == 0) {
      base.group_by;
    } else if (List.length(base.group_by) == 0) {
      userClauses;
    } else {
      userClauses
      |> List.rev
      |> List.tl
      |> List.fold_left((acc, x) => group_by(x, acc), base)
      |> (x => x.group_by);
    }
  );

let mergeOrderBy = (baseClauses, userClauses) =>
  if (List.length(userClauses) == 0) {
    baseClauses;
  } else if (List.length(baseClauses) == 0) {
    userClauses;
  } else {
    userClauses
    |> List.rev
    |> List.tl
    |> (x => List.concat([x, [","], baseClauses]));
  };

let mergeWhere = (base, userClauses) =>
  SqlComposer.Select.(
    userClauses
    |> List.rev
    |> List.tl
    |> List.fold_left((acc, x) => where(x, acc), base)
    |> (x => x.where)
  );

let mergeFields = (base, userClauses) =>
  userClauses
  |> List.fold_left((acc, x) => SqlComposer.Select.field(x, acc), base)
  |> (x => x.fields);

let factory = (table, base, user) =>
  SqlComposer.Select.(
    {
      modifier: base.modifier != None ? base.modifier : user.modifier,
      fields: mergeFields(base, user.fields),
      from: [{j|FROM `$table`|j}],
      join: List.concat([user.join, base.join]),
      where: mergeWhere(base, user.where),
      order_by: mergeOrderBy(base.order_by, user.order_by),
      group_by: mergeGroupBy(base, user.group_by),
      limit: List.length(base.limit) > 0 ? base.limit : user.limit,
    }
    |> to_sql
  );

/* Public */
let make = (table, base, user) =>
  factory(
    table,
    base(SqlComposer.Select.select),
    user(SqlComposer.Select.select),
  );
