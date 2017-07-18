var net = require('net');
var HOST = '127.0.0.1';
var PORT = 3000;

// サーバーインスタンスを生成し、リッスンします
// net.createServer()に渡す関数は、'connection'イベントハンドラーになります。
// コールバック関数が受け取るsockeオブジェクトは各接続ごとにユニークなものとなります。
net.createServer(function(sock) {
    // TCPサーバーが接続しました。socketオブジェクトが自動的に割り当てられます。
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);
    sock.on('data', function(data){
        // console.log("length:" + data.length);
        // console.log(data.toString('UTF-8', 1, data.length));
        var jsondata = JSON.parse(data);
        console.log(jsondata);
        try{
            console.log(jsondata.nowPosition.x);
        }catch(e){
            console.log("json parse error.")
        }
    });
    sock.on('close', function(had_err){
    });
    // 'error'イベントハンドラー
    sock.on('error', function(err) {
        console.log('ERROR: ' + err.stack);
    });
}).listen(PORT, HOST);
console.log('Server listening on ' + HOST +':'+ PORT);
