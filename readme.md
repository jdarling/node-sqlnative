sqlnative2.node - Native connection to SQL Server for Node.js
============
Installation

    Download from GitHub and extract to your node-modules folder.

What is sqlnative2?
------------------
sqlnative2.node is a native (binary) library for Node.js that allows you to connect to SQL Server and execute SQL Statements (Stored Procedures, Queries, whatever can be done through a command object).  It was developed due to the lack of working SQL clients for Node.js.  The code is C++ and compiles in Visual Studios 2010 (professional required for 64 bit).  The current library contains the 64 bit build of sqlnative2.node

Example:

```javascript
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
```
