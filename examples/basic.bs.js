'use strict';

var Curry     = require("bs-platform/lib/js/curry.js");
var Mysql     = require("bs-mysql2/src/mysql.bs.js");
var Pimpmysql = require("../src/pimpmysql.bs.js");

var db = Mysql.Connection[/* make */0](/* Some */["127.0.0.1"], /* Some */[3306], /* Some */["root"], /* None */0, /* None */0, /* () */0);

Curry._2(Mysql.Promise[/* Connection */0][/* end_ */0], db, Pimpmysql.raw(db, "SELECT ? + ? AS result", /* Some */[/* int array */[
                5,
                6
              ]], /* () */0).then((function (res) {
              console.log(res);
              return Promise.resolve(1);
            }))).catch((function (err) {
        console.log(err);
        Mysql.Connection[/* end_ */1](db);
        return Promise.resolve(-1);
      }));

exports.db = db;
/* db Not a pure module */
