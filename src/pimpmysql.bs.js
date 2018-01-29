'use strict';

var Util      = require("util");
var Curry     = require("bs-platform/lib/js/curry.js");
var Debug     = require("bs-node-debug/src/Debug.bs.js");
var Mysql     = require("bs-mysql2/src/mysql.bs.js");
var Chalk     = require("chalk");
var Js_option = require("bs-platform/lib/js/js_option.js");

var debug = Debug.make("PIMPMYSQL", "QUERY");

function logThenResolve(db, sql, params) {
  var sqlLabel = Chalk.blue("SQL:");
  var paramsLabel = Chalk.blue("PARAMS:");
  var paramsString = Util.inspect(Js_option.getWithDefault(/* array */[], params));
  Curry._1(debug, "" + (String(sqlLabel) + (" " + (String(sql) + (" \n" + (String(paramsLabel) + (" " + (String(paramsString) + ""))))))));
  return Promise.resolve(db);
}

function raw(db, sql, params, _) {
  return Mysql.Promise[/* pquery */3](sql, params, logThenResolve(db, sql, params));
}

exports.raw = raw;
/* debug Not a pure module */
