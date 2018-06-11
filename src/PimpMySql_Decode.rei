let oneRow:
  (Js.Json.t => 'a, (array(Js.Json.t), 'b)) =>
  Future.t(Belt.Result.t(option('a), exn));

let rows:
  (Js.Json.t => 'a, (array(Js.Json.t), 'b)) =>
  Future.t(Belt.Result.t(array('a), exn));
