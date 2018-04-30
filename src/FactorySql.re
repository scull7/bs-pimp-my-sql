/* Private */
let removeHeadOfList = list =>
  switch (List.tl @@ list) {
  | exception _ => []
  | result => result
  };

let mergeGroupBy = (base, userClauses) =>
  SqlComposer.Select.(
    userClauses
    |> List.rev
    |> removeHeadOfList
    |> List.fold_left((acc, x) => group_by(x, acc), base)
    |> (x => x.group_by)
  );

let mergeOrderBy = (baseClauses, userClauses) =>
  if (List.length(userClauses) == 0) {
    baseClauses;
  } else if (List.length(baseClauses) == 0) {
    userClauses;
  } else {
    userClauses
    |> List.rev
    |> removeHeadOfList
    |> (x => List.concat([x, [","], baseClauses]));
  };

let mergeWhere = (base, userClauses) =>
  SqlComposer.Select.(
    userClauses
    |> List.rev
    |> removeHeadOfList
    |> List.fold_left((acc, x) => where(x, acc), base)
    |> (x => x.where)
  );

let mergeFields = (base, userClauses) =>
  userClauses
  |> List.fold_left((acc, x) => SqlComposer.Select.field(x, acc), base)
  |> (x => x.fields);

let factory = (table, base, user) =>
  SqlComposer.Select.{
    modifier: base.modifier != None ? base.modifier : user.modifier,
    fields: mergeFields(base, user.fields),
    from: [{j|FROM `$table`|j}],
    join: List.concat([user.join, base.join]),
    where: mergeWhere(base, user.where),
    order_by: mergeOrderBy(base.order_by, user.order_by),
    group_by: mergeGroupBy(base, user.group_by),
    limit: List.length(base.limit) > 0 ? base.limit : user.limit,
  };

/* Public */
/* @TODO - the base and user functions don't necessarily need to user
   the SqlComposer.Select.select type defined locally, figure out
   how to change this so that we will always know exactly what goes
   into the base and user function.*/
let make = (table, base, user) => factory(table, base, user);
