open Jest;

type animal = {type_: string};

external animalToJson : animal => Js.Json.t = "%identity";

let table = "animal";

describe("FactorySql", () => {
  test("make (merging all clauses from base and user)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
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
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  animal.id",
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
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
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
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  animal.id",
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
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
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
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  animal.id",
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
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
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
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (no user where clause)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
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
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ",",
          "  foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (no base order by clause)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> group_by("animal.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
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
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (no user order by clause)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> group_by("foo.id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          "GROUP BY",
          "  animal.id",
          ",   foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (no base group by clause)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> group_by("foo.id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          "GROUP BY",
          "  foo.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (no user group by clause)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
        |> limit(~offset="0", ~row_count=Some(1))
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          "GROUP BY",
          "  animal.id",
          "LIMIT 1 OFFSET 0",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
  test("make (no base limit clause)", () => {
    let base =
      SqlComposer.Select.(
        select
        |> field("animal.id")
        |> join("JOIN blah ON blah.id = animal.blah_id")
        |> where("AND animal.deleted IS NULL")
        |> order_by(`Asc("animal.id"))
        |> group_by("animal.id")
      );
    let user =
      SqlComposer.Select.(
        select
        |> modifier(`Distinct)
        |> field("foo.id")
        |> join("JOIN foo ON foo.id = blah.foo_id")
        |> limit(~offset="5", ~row_count=Some(1))
      );
    let output = FactorySql.make(table, base, user);
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          "  ,   foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          "GROUP BY",
          "  animal.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    output == expected ? pass : fail("not expected output");
  });
});
