let db =
  Mysql.Connection.make(~host="127.0.0.1", ~port=3306, ~user="root", ());

Pimpmysql.raw(~db, ~sql="SELECT ? + ? AS result", ~params=[|5, 6|], ())
|> Js.Promise.then_(res => {
     Js.log(res);
     Js.Promise.resolve(1);
   })
|> Mysql.Promise.Connection.end_(db)
|> Js.Promise.catch(err => {
     Js.log(err);
     Mysql.Connection.end_(db);
     Js.Promise.resolve(-1);
   });
