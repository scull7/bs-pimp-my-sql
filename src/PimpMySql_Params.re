let named = json => json |. MySql2.Params.named |. Some;

let positional = arrayOfJson =>
  arrayOfJson |. Json.Encode.jsonArray |. MySql2.Params.positional |. Some;
