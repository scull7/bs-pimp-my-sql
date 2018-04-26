let oneRow: (
  Js.Json.t => 'a,
  (array(Js.Json.t), 'b)
) => option('a);

let rows: (
  Js.Json.t => 'a,
  (array(Js.Json.t), 'b)
) => array('a);
