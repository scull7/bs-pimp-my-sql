open Jest;

let table = "animal";

describe("Fake Test", () =>
  test("fake test", () =>
    pass
  )
);

describe("PimpMySql_FactorySql", () => {
  test("make (merging all clauses from base and user)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. where("AND foo.deleted IS NULL")
        |. orderBy(`Asc("foo.id"))
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ", foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ", foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (with base modifier)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. modifier(`Distinct)
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. where("AND foo.deleted IS NULL")
        |. orderBy(`Asc("foo.id"))
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ", foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ", foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (with user modifier)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. where("AND foo.deleted IS NULL")
        |. orderBy(`Asc("foo.id"))
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ", foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ", foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no base where clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. where("AND foo.deleted IS NULL")
        |. orderBy(`Asc("foo.id"))
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND foo.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ", foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ", foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no user where clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. orderBy(`Asc("foo.id"))
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          ", foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ", foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no base order by clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. orderBy(`Asc("foo.id"))
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  foo.id ASC",
          "GROUP BY",
          "  animal.id",
          ", foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no user order by clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          "GROUP BY",
          "  animal.id",
          ", foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no base group by clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. groupBy("foo.id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
          "FROM `animal`",
          "JOIN blah ON blah.id = animal.blah_id",
          "JOIN foo ON foo.id = blah.foo_id",
          "WHERE 1=1",
          "AND animal.deleted IS NULL",
          "ORDER BY",
          "  animal.id ASC",
          "GROUP BY",
          "  foo.id",
          "LIMIT 1 OFFSET 5",
        ],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no user group by clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
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
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no base limit clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
        |. limit(~offset=5, ~row_count=1)
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
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
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (no user limit clause)", () => {
    let base =
      SqlComposer.Select.(
        make()
        |. field("animal.id")
        |. join("JOIN blah ON blah.id = animal.blah_id")
        |. where("AND animal.deleted IS NULL")
        |. orderBy(`Asc("animal.id"))
        |. groupBy("animal.id")
        |. limit(~offset=0, ~row_count=1)
      );
    let user = base =>
      SqlComposer.Select.(
        base
        |. modifier(`Distinct)
        |. field("foo.id")
        |. join("JOIN foo ON foo.id = blah.foo_id")
      );
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. user
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        [
          "SELECT DISTINCT",
          "  animal.id",
          ", foo.id",
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
    Expect.expect(output) |> Expect.toBe(expected);
  });
  test("make (only select)", () => {
    let base = SqlComposer.Select.(make() |. field("animal.id"));
    let output =
      PimpMySql_FactorySql.make(table, base)
      |. SqlComposer.Select.field("animal.type")
      |. SqlComposer.Select.toSql;
    let expected =
      String.concat(
        "\n",
        ["SELECT", "  animal.id", ", animal.type", "FROM `animal`"],
      );
    Expect.expect(output) |> Expect.toBe(expected);
  });
});
