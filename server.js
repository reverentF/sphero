var net = require('net');
var HOST = '127.0.0.1';
var PORT = 3000;

//init sphero
var sphero = require("sphero"),
    sprk = sphero("92edbec1855745bb85bb389a9fe54907"); // change BLE address accordingly

sprk.connect(function() {
    console.log("Connecting to sphero is suceeded.")
    sprk.color("green");
    // roll BB-8 in a random direction, changing direction every second
    /*
    setInterval(function() {
        var direction = Math.floor(Math.random() * 360);
        sprk.roll(150, direction);
    }, 1000);
    */
});

function parseData(data){
    try {
        var jsondata = JSON.parse(data);
        console.log(jsondata);
        return jsondata;
    }catch(e){
        console.log("json parse error.")
        return null;
    }
}

function receive(sprk, data){
    var positionData = parseData(data);
    if(positionData){
        var nowPosition = positionData.nowPosition;
        var targetPosition = positionData.targetPosition;

        var diff_x = targetPosition.x - nowPosition.x;
        var diff_y = targetPosition.y - nowPosition.y;

        var distance = Math.sqrt(diff_y*diff_y + diff_x*diff_x);
        var direction = Math.atan2(diff_y, diff_x) * (180 / Math.PI);
        if(direction < 0){
            direction += 360;
        }

        var threshold = 50; 
        var speed = 10;

        if(distance > threshold){
            sprk.roll(speed, direction);
            console.log('direction ' + direction);
            console.log('distance ' + distance);
        }
        console.log('positionData received:' + positionData)
    }
}

function close(sprk){
    sprk.disconnect(function(){
        console.log('Now disconnected from Sphero.');
    });
}

// カメラからspheroの位置情報を受診するサーバー
net.createServer(function(sock) {
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
    sock.on('data', function(data){
        receive(sprk, data);
    });
    sock.on('close', function(had_err){
        close(sprk);
    });
    sock.on('error', function(err) {
        console.log('ERROR: ' + err.stack);
    });
}).listen(PORT, HOST);
console.log('Server listening on ' + HOST +':'+ PORT);
