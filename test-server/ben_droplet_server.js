// Create server
const  express = require('express');
const  bodyParser = require('body-parser');
let port = process.env.PORT || 8080;
let app = express();
let server = require('http').createServer(app).listen(port, function () {
  console.log('Server listening at port: ', port);
});


app.use(express.static('public'));

app.use(bodyParser.json());
//source: https://expressjs.com/en/resources/middleware/body-parser.html#bodyparserurlencodedoptions
app.use(bodyParser.urlencoded({ extended: false }));



app.get("/", (req, res) => {
  console.log("It got get-ted!")
});

app.get("/api/route", (req, res) => {
  console.log("It got get-ted!")
});

app.post("/", (req, res) => {
  console.log("It got posted!")
  console.log(req.body)
  res.status(200).end();
});

app.post("/post-test", (req, res) => {
  console.log("It got posted to the test!")
  console.log(req.body);
res.status(200).end();
});
