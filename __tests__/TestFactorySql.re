open Jest;

type animal = {type_: string};

external animalToJson : animal => Js.Json.t = "%identity";

let table = "animal";

describe("FactorySql", () => {
  test("make (merging all clauses from base and user)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("test.id")
        |> join("JOIN blah ON blah.id = test.blah_id")
        |> where("AND test.deleted IS NULL")
        |> order_by(`Asc("test.id"))
        |> group_by("test.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> where("AND foo.deleted IS NULL")
        |> order_by(`Asc("foo.id"))
        |> group_by("foo.id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT",
          "  test.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = test.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND test.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  test.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  test.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (base modifier)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("test.id")
        |> join("JOIN blah ON blah.id = test.blah_id")
        |> where("AND test.deleted IS NULL")
        |> order_by(`Asc("test.id"))
        |> group_by("test.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> where("AND foo.deleted IS NULL")
        |> order_by(`Asc("foo.id"))
        |> group_by("foo.id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  test.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = test.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND test.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  test.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  test.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (user modifier)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("test.id")
        |> join("JOIN blah ON blah.id = test.blah_id")
        |> where("AND test.deleted IS NULL")
        |> order_by(`Asc("test.id"))
        |> group_by("test.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> where("AND foo.deleted IS NULL")
        |> order_by(`Asc("foo.id"))
        |> group_by("foo.id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  test.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = test.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND test.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  test.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  test.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (no base where clause)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("test.id")
        |> join("JOIN blah ON blah.id = test.blah_id")
        |> order_by(`Asc("test.id"))
        |> group_by("test.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> where("AND foo.deleted IS NULL")
        |> order_by(`Asc("foo.id"))
        |> group_by("foo.id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  test.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = test.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  test.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  test.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
});
