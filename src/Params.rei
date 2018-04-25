let named: Js.Json.t => option([> `Named(Js.Json.t) ]);

let positional : Js.Json.t => option([> `Positional(Js.Json.t) ]);
