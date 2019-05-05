var express = require("express");
var app = express();
var server = require("http").Server(app);
var io =require("socket.io")(server);
var mysql = require('mysql');
var mqtt  = require('mqtt');
var options = {
  username: 'ducmqtt',
  password: 'duc'
}

var client = mqtt.connect('mqtt://localhost:1883/', options);
var toppic1 = 'Toppic 1';
var message = "test message";

function publish(topic,msg,options){
	console.log("publishing",msg);
	if (client.connected == true){
		client.publish(topic,msg,options);
	}
}

app.use(express.static("public"));
app.set("view engine", "ejs");
app.set("views","./views");

server.listen(3000, function(){
    console.log('http://localhost:3000/');
  });

app.get("/", function(req, res){
    res.render("home");
});

io.on("connection",function(socket){
  console.log("User: " + socket.id + " connection!");
  socket.on("disconnect", function(){
    console.log("User: " + socket.id + " disconnected!");
  });

  socket.on("control-led",function(data){
    publish(toppic1,data.toString(),options);
  })
})

var con = mysql.createConnection({
  host: "localhost",
  user: "ducsql",
  password: "duc",
  database: "ducdb"
  });
  con.connect(function(error){
    if(!!error){
      console.log('Error connect to Mysql!');
    }else{
      console.log('Connected to Mysql!');
    }
  });
  app.get('/room1', function(req, res){
    con.query("SELECT * FROM room1 where id = (select max(id) from room1)", function (err, result, fields) {
    if (err) throw err;
    con.query("select max(Temperature) as Temperature from room1", function (err, maxtemp, fields) {
      if (err) throw err;
      con.query("select max(Humidity) as Humidity from room1", function (err, maxhum, fields) {
        if (err) throw err;
          con.query("select max(Dust) as Dust from room1", function (err, maxdust, fields) {
            if (err) throw err;
            con.query("select min(Temperature) as Temperature from room1", function (err, mintemp, fields) {
              if (err) throw err;
              con.query("select min(Humidity) as Humidity from room1", function (err, minhum, fields) {
                if (err) throw err;
                  con.query("select min(Dust) as Dust from room1", function (err, mindust, fields) {
                    if (err) throw err;
                    con.query("select round((select avg(Temperature) from room1),1) AS avgtemp FROM room1 limit 0,1", function (err, avgtemp, fields) {
                      if (err) throw err;
                      con.query("select round((select avg(Humidity) from room1),1) AS avghum FROM room1 limit 0,1", function (err, avghum, fields) {
                        if (err) throw err;
                          con.query("select round((select avg(Dust) from room1),1) AS avgdust FROM room1 limit 0,1", function (err, avgdust, fields) {
                            if (err) throw err;
                            var a = {result: result[0], 
                                    maxtemp: maxtemp[0], maxhum: maxhum[0], maxdust: maxdust[0],
                                    mintemp: mintemp[0], minhum: minhum[0], mindust: mindust[0],
                                    avgtemp: avgtemp[0], avghum: avghum[0], avgdust: avgdust[0]
                                  };
                                  res.json(a);
                    });
                  });
                });
              });
            });
          });
        });
      });
    });
  });
});