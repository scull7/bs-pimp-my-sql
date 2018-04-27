/* Private */
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

let merge = (base, user, baseClauses, userClauses, fn) =>
  if (List.length(userClauses) == 0) {
    base;
  } else if (List.length(baseClauses) == 0) {
    user;
  } else {
    userClauses
    |> List.rev
    |> List.tl
    |> List.fold_left((acc, x) => fn(x, acc), base);
  };

let mergeFields = (base, clauses) =>
  List.fold_left(
    (acc, x) => SqlComposer.Select.field(x, acc),
    base,
    clauses,
  );

let factory = (table, base, user) =>
  SqlComposer.Select.(
    {
      modifier: base.modifier != None ? base.modifier : user.modifier,
      fields: mergeFields(base, user.fields).fields,
      from: [{j|FROM `$table`|j}],
      join: List.concat([user.join, base.join]),
      where: merge(base, user, base.where, user.where, where).where,
      order_by: mergeOrderBy(base.order_by, user.order_by),
      group_by:
        merge(base, user, base.group_by, user.group_by, group_by).group_by,
      limit: List.length(base.limit) > 0 ? base.limit : user.limit,
    }
    |> to_sql
  );

/* Public */
let make = (table, base, user) => factory(table, base, user);
