var sphero = require("sphero"),
    bb8 = sphero("92edbec1855745bb85bb389a9fe54907"); // change BLE address accordingly

bb8.connect(function() {
  // roll BB-8 in a random direction, changing direction every second
  setInterval(function() {
    var direction = Math.floor(Math.random() * 360);
    bb8.roll(150, direction);
  }, 1000);
});
