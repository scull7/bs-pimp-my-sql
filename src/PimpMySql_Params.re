let named = json => Some(`Named(json));

let positional = arrayOfJson =>
  Some(`Positional(Json.Encode.jsonArray(arrayOfJson)));
