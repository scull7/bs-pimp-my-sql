[@bs.module "util"] external inspect : 'a => string = "";

module Operators = {
  let (>>=) = (x, fn) => Js.Promise.then_(y => Js.Promise.resolve(fn(y)), x);
};
