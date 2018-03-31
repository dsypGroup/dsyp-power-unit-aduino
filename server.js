var express = require("express");
var mysql = require('mysql');
var app = express();
var bodyParser = require("body-parser");
//var http = require('http');
var isBulkReq = true;

app.use(function (req, res, next) {
    res.header("Access-Control-Allow-Origin", "http://localhost:4200");
    //res.header("Access-Control-Allow-Origin", "http://ec2-18-217-238-61.us-east-2.compute.amazonaws.com");
    res.header('Access-Control-Allow-Methods', 'PUT, GET, POST, DELETE, OPTIONS');
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    res.setHeader('Access-Control-Allow-Credentials', true);
    next();
});

app.use(bodyParser.urlencoded({ extended: false }));
app.use(bodyParser.json());

//var server = http.createServer(app);

var pool = mysql.createPool({
    connectionLimit: 100, //important
    host: 'localhost',
    user: 'root',
    password: '123456',
    database: 'dsypapplication',
    debug: false
});

function intervalFunc() {
    isBulkReq = true;
}

setInterval(intervalFunc, 60000*2);

function handle_database(req, res, str) {
    pool.getConnection(function (err, connection) {
        if (err) {
            //connection.release();
            res.json({"code": 100, "status": "Error in connection database"});
            return;
        }

        console.log('connected as id ' + connection.threadId + str);

        connection.query(str, function (err, rows) {
            connection.release();
            if (!err) {
                res.send(JSON.stringify({
                    manage: rows
                }), 200);
            }
        });

        connection.on('error', function (err) {
            res.json({"code": 100, "status": "Error in connection database"});
            return;
        });
    });
}

function handle_databaseDevices(req, res, str) {
    pool.getConnection(function (err, connection) {
        if (err) {
            //connection.release();
            res.json({"code": 100, "status": "Error in connection database"});
            return;
        }

        console.log('connected as id ' + connection.threadId + str);

        connection.query(str, function (err, rows) {
            connection.release();
            if (!err) {
                res.send(JSON.stringify({
                    devices: rows
                }), 200);
            }
        });

        connection.on('error', function (err) {
            res.json({"code": 100, "status": "Error in connection database"});
            return;
        });
    });
}

function handle_getBulbValues(req, res, str) {
    pool.getConnection(function (err, connection) {
        if (err) {
            //connection.release();
            res.json({"code": 100, "status": "Error in connection database"});
            return;
        }

        //console.log('connected as id ' + connection.threadId + str);

        connection.query(str, function (err, rows) {
            connection.release();
            if (!err) {
                var devices = {};
                if (isBulkReq) {
                    devices.isBulkReq = 'isBulkReq-true';
                    isBulkReq = false;
                } else {
                    devices.isBulkReq = 'isBulkReq-false';
                }

                for (var i=0; i<rows.length; i++) {
                    devices[rows[i].devicePort] = rows[i].devicePort + "-" + rows[i].deviceStatus;
                }

                console.log("Device Status message" + JSON.stringify(devices));
                res.send(JSON.stringify(devices), 200);
            }
        });

        connection.on('error', function (err) {
            res.json({"code": 100, "status": "Error in connection database"});
            return;
        });
    });
}

app.get("/api/manages", function (req, res) {
    var str = "select * from users";
    handle_database(req, res, str);
});

app.get("/api/devices", function (req, res) {
    var str = "select * from devices";
    console.log('req: '+ req.params.id);
    handle_databaseDevices(req, res, str);
});

app.get("/api/devices/:id", function (req, res) {
    var str = "select * from devices";
    handle_databaseDevices(req, res, str);
});

app.post('/api/devices',function(req,res){
    console.log('post request came');
    var query1=req.body.device.deviceName;
    var query2=req.body.device.devicePort;
    console.log('post request came ' +query1+ ' ===' + query2);

    pool.getConnection(function (err, connection) {
        connection.query('INSERT INTO devices (deviceName, deviceGroup, deviceStatus, unitLimit, deviceUnits, id, devicePort) VALUES (?,?,?,?,?,?,?)', [req.body.device.deviceName, req.body.device.deviceGroup, req.body.device.deviceStatus, req.body.device.unitLimit, req.body.device.deviceUnits, req.body.device.id, req.body.device.devicePort], function (error, results, fields) {
            if (error) {
                console.log("Error in post/api/devices : " + error);
            } else {
                //console.log("resulsts : " + JSON.stringify(results));
                res.end(JSON.stringify(results));
            }
        });
    });

    res.end("yes");
});

app.post('/api/login',function(req,res){
    console.log('post request came from login');
    var query1=req.body.username;
    var query2=req.body.password;
    console.log('post request came ' +query1+ ' ===' + query2);

    try {
        pool.getConnection(function (err, connection) {
            var reqPassword = req.body.password;
            connection.query('select * from users WHERE username=?', [req.body.username], function (error, results, fields) {
                try{
                    if (error) {
                        res.end(JSON.stringify(0));
                    } else {
                        console.log("resulsts : " + JSON.stringify(results));
                        var authMessage = 0;

                        console.log('reqPassword :' + reqPassword);
                        console.log('rdbPassword :' + results[0].password);
                        if(results[0].password == reqPassword) {
                            authMessage = 200;
                        } else {
                            authMessage = 0;
                        }

                        res.end(JSON.stringify(authMessage));
                    }
                }catch (e) {
                    console.log("Error Occureed");
                }
            });
        });
    } catch (e) {
        res.end(JSON.stringify(0));
    }
});

app.get('/api/testArduino',function(req,res){
    //console.log('get request came' + req);

    //var str = "SELECT * FROM 'devices' WHERE 'deviceName'='bulb2'";
    //handle_getBulbValues(req,res,str);

    var str = "select * from devices";
    //console.log('req: '+ req.params.id);
    handle_getBulbValues(req, res, str);
});

app.post('/api/testArduino', function (req, res) {
    console.log('device name ' + req.body.deviceMessage);
    var deviceMessage = req.body.deviceMessage;
    var deviceArray = deviceMessage.split(',');

    for (var i = 0; i < deviceArray.length; i++) {
        if (deviceArray[i]) {
            updateDataBaseUnits(deviceArray[i]);
        }
    }

    res.end(JSON.stringify("Yes"));
});

function updateDataBaseUnits(device) {
    var deviceArr = device.split(':');
    var portName = deviceArr[0];
    var deviceUnits = deviceArr[1];

    console.log('device portname ' + portName);
    console.log('device units ' + deviceUnits);

    if (portName && deviceUnits) {
        pool.getConnection(function (err, connection) {
            connection.query('UPDATE devices SET deviceUnits=? where devicePort=?', [deviceUnits,portName], function (error, results, fields) {
                try {
                    if (error) {
                        console.log('error Occured' + error);
                    }
                } catch (e) {
                    console.log("error occured"+ e);
                }
            });
        });
    }
}


//app.put('/api/devices/:id',function(req,res){
//    console.log('post request came');
//    var query1=req.body.device.deviceName;
//    var query2=req.body.device.deviceUnits;
//    console.log('post request came ' +query1+ ' ===' + query2);
//    res.end("yes");
//});

app.get("/api", function (req, res) {
    var str = "select * from users";
    //handle_database(req, res, str);
    console.log('str: + ' + str);
});

app.get("/", function (req, res) {
    res.json({"code": 100, "status": "Error in connection database"});
});

app.get('/:id', function (req, res) {
    res.send({
        'manage': {
            id: req.params.id
        }
    });
});

//rest api to update record into mysql database
app.put('/api/devices/:id', function (req, res) {

    //console.log("params: "+req.body.device.deviceStatus+"  "+req.body.device.deviceName);
    pool.getConnection(function (err, connection) {
        connection.query('UPDATE devices SET deviceStatus=? where deviceName=?', [req.body.device.deviceStatus,req.body.device.deviceName], function (error, results, fields) {
            if (error) throw error;
            //console.log("resulsts : " + JSON.stringify(results));
            res.end(JSON.stringify(results));
        });
    });
});

app.delete('/api/devices/:id', function (req, res) {
    console.log('came request thant to options froddm thereequavale'+req.params.id);

    pool.getConnection(function (err, connection) {
        connection.query('DELETE from devices WHERE id=?', [req.params.id], function (error, results, fields) {
            if (error) throw error;
            //console.log("resulsts : " + JSON.stringify(results));
            res.end(JSON.stringify(200));
        });
    });
});

var PORT = 3000;

app.listen(PORT, function (err) {
    if (err) {
        console.log(err);
    } else {
        console.log('Working on port ' + PORT);
    }
});