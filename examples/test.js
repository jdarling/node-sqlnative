console.log("Loading SQL Server driver");
var cs = require('../Debug/sqlnative2');

console.log("Connecting to DB Server");
var conn = new cs.SQLConnection('Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=TEST;Data Source=localhost:1443');

console.log(new Array(40).join('-='));
console.log("Running a simple query against the server");
console.log(conn.query("EXEC [dbo].[TestProc]").length);

console.log(new Array(40).join('-='));
console.log("Now to break it by connecting to something that doesn't exist");
try{
  var conn2 = new cs.SQLConnection('Integrated Security=SSPI;Persist Security Info=False;Initial Catalog=NOEXIST;Data Source=goBoom');
}catch(e){
  console.log("Got an error: ", e);
}

console.log(new Array(40).join('-='));
console.log("Try to brake it by sending a bad query");
try{
  conn.query("FRAZ BATZ NIMRO FREEBAR")
}catch(e){
  console.log("Got an error: ", e);
}

console.log(new Array(40).join('-='));
console.log("Try to brake it by selecting from a table that does not exist");
try{
  console.log(conn.query("SELECT * FROM this_table_does_not_exist"));
}catch(e){
  console.log("Got an error: ", e);
}

console.log(new Array(40).join('-='));
console.log("Finally re-run the query just to prove we still work");
console.log(conn.query("EXEC [dbo].[TestProc]"));