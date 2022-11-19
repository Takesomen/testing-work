const usersApi = require("bindings")("users");
const express = require("express");
const path = require("path");
const bodyParser = require("body-parser");
const open = require("open");

const app = express();

app.set("view engine", "pug");
app.set("views", path.join(__dirname, "views"));
app.use("/static", express.static(path.join(__dirname, "public")));

const urlencodedParser = bodyParser.urlencoded({ extended: false });

app.get("/", (_request, response) => {
  response.render("index");
});

app.post("/", urlencodedParser, (request, response) => {
  if (!request.body) return response.sendStatus(400);
  const users = usersApi.getAllUsers().map((u) => u.toLowerCase());
  const user = request.body.user;
  response.render("result", {
    result: users.includes(user?.toLowerCase()),
    user: user,
  });
});

const port = parseInt(process.env.PORT) || 3000;
app.listen(port, () => {
  open(`http://localhost:${port}`);
  console.log(`Server starts in ${port}`);
});
