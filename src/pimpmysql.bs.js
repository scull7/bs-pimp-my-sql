'use strict';

var Mysql = require("bs-mysql2/src/mysql.bs.js");

function raw(db, sql, params, _) {
  return Mysql.Promise[/* pquery */3](sql, params, Promise.resolve(db));
}

exports.raw = raw;
/* Mysql Not a pure module */
