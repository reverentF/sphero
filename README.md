# Spehro

Controll Sphero using [Sphero.js](https://github.com/orbotix/sphero.js)

## Connect to Sphero SPRK+ from OSX

1. determine Sphero's UUID

  ``` sh
  $ node ./node_modules/noble/examples/advertisement-discovery.js
  peripheral discovered (944f561f8cf441f3b5405ed48f5c63cf with address <unknown, unknown>, connectable true, RSSI -73:
      hello my local name is:
          BB-131D
      can I interest you in any of the following advertised services:
          []
      here is my manufacturer data:
          "3330"
      my TX power level is:
          -18
  ```
  In the above output, the device UUID is 944f561f8cf441f3b5405ed48f5c63cf.

2. set UUID to `.env`

## 外部カメラからのspheroの制御

### Usage
1. `node server.js`

2. 外部カメラからSpheroをトラッキングするプログラムを作成

3. `localhost:3000`に対してTCP接続

4. Spheroの現在位置と目標位置をJSON形式で送信
``` json
{
	"nowPosition": {
		"x": 0,
		"y": 0
	},
	"targetPosition": {
		"x": 100,
		"y": -50
	}
}
```
