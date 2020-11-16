// Create server
let port = process.env.PORT || 8000;
let express = require('express');
let app = express();
let server = require('http').createServer(app).listen(port, function () {
  console.log('Server listening at port: ', port);
});


app.use(express.static('public'));

app.use(express.json());

app.get("/", (req, res) => {
  console.log("It got get-ted!")
});

app.get("/api/route", (req, res) => {
  console.log("It got get-ted!")
});

app.post("/", (req, res) => {
  console.log("It got posted!")
  console.log(req.body)
  res.sendStatus(200);
});

app.post("/post-test", (req, res) => {
  console.log("It got posted to the test!")
  console.log(req.body);
});
