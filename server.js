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

function parse_command(data){
    return "ok";
}

function receive(sprk, data){
    var command = parse_command(data);
    if(command){
        var direction = Math.floor(Math.random() * 360);
        sprk.roll(1000, 10);
        console.log('command received:' + command)
    }
}

function close(sprk){
    sprk.disconnect(function(){
        console.log('Now disconnected from Sphero.');
    });
}

// サーバーインスタンスを生成し、リッスンします
// net.createServer()に渡す関数は、'connection'イベントハンドラーになります。
// コールバック関数が受け取るsockeオブジェクトは各接続ごとにユニークなものとなります。
net.createServer(function(sock) {
    // TCPサーバーが接続しました。socketオブジェクトが自動的に割り当てられます。
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
    sock.on('data', function(data){
        receive(sprk, data);
    });
    sock.on('close', function(had_err){
        close(sprk);
    });
    // 'error'イベントハンドラー
    sock.on('error', function(err) {
        console.log('ERROR: ' + err.stack);
    });
}).listen(PORT, HOST);
console.log('Server listening on ' + HOST +':'+ PORT);
