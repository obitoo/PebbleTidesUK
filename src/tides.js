

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    // Get the initial weather
    getTides();
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    console.log("AppMessage received!");
    getTides();
  }                     
);

var xhrRequest = function (url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
};


function getTides(pos) {
  // Construct URL
  var url = "http://82.69.65.202:8080/tides0110.json"; //desktop 8080

  // Send request to My Server
  
  // TODO - what if fails?
  
  xhrRequest(url, 'GET', 
          function(responseText) {
            // responseText contains a JSON object with weather info
            var json = JSON.parse(responseText);
      
            // get next tide
            var state0 = json.tides[0].state;
            var time0  = json.tides[0].time;
            var state1 = json.tides[1].state;
            var time1  = json.tides[1].time;
            var state2 = json.tides[2].state;
            var time2  = json.tides[2].time;
            var state3 = json.tides[3].state;
            var time3  = json.tides[3].time;
//             var state2 = "x";
//             var time2  = "x";
//             var state3 = "x";
//             var time3  = "x";
            
            console.log("0 - state is " + state0 + "time is"+time0);
            console.log("1 - state is " + state1 + "time is"+time1);
            console.log("2 - state is " + state2 + "time is"+time2);
            console.log("3 - state is " + state3 + "time is"+time3);
 
      
            // build dict to send
            var dictionary = {
              "KEY_STATE_0": state0,
              "KEY_TIME_0": time0,
              "KEY_STATE_1": state1,
              "KEY_TIME_1": time1,
              "KEY_STATE_2": state2,
              "KEY_TIME_2": time2,
              "KEY_STATE_3": state3,
              "KEY_TIME_3": time3
            };
      
            // Send to Pebble
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Tide info sent to Pebble successfully!");
                 },
                function(e) {
                    console.log("Error sending tides info to Pebble!");
                }
            );
          }      
    );
}



