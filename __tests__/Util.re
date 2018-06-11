let testFutureResult = (name, runner) =>
  Jest.testPromise(name, () =>
    Js.Promise.make((~resolve, ~reject) =>
      runner()
      |. Future.get(
           fun
           | Belt.Result.Ok(x) => resolve(. x)
           | Belt.Result.Error(e) => reject(. e),
         )
      |. ignore
    )
  );
