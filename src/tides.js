


  //
  //  Init   ======================================
  //

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
    console.log("PebbleKit JS ready!");
    // Get the initial weather
    getTides();
  }
);



  //
  //  Config   ======================================
  //

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  Pebble.openURL('http://82.69.65.202:8080/config.html');
});


// config page - close
Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  var config = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(config));
  
  // Send to Pebble - add MSG_TYPE to allow routing
  var dictionary = {
              "MSG_TYPE"        :"config",
              "CFG_INVERT_COL"  : config.cfg_invert_col,
              "CFG_SHOW_HEIGHTS": config.cfg_show_heights,
              "CFG_LINE_GRAPH"  : config.cfg_line_graph
    
  };
  console.log("Message = " + JSON.stringify(dictionary));
  
  Pebble.sendAppMessage(dictionary,
                        function(e) {
                          console.log("Config sent to Pebble successfully");
                        },
                        function(e) {
                          console.log("Error sending Config to Pebble!");
                        }
                       );
});




  //
  //  Tides   ============================
  //

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
  // Construct URL - TODO - dns
  var url = "http://82.69.65.202:8080/tides0110.json"; //desktop 8080

  // Send request to My Server
  
  // TODO - what if fails?
  
  xhrRequest(url, 'GET', 
          function(responseText) {
            // responseText contains a JSON object with weather info
            var json = JSON.parse(responseText);
      
            // get next tide
            var state0  = json.tides[0].state;
            var state1  = json.tides[1].state;
            var state2  = json.tides[2].state;
            var state3  = json.tides[3].state;

            var time0   = json.tides[0].time;
            var time1   = json.tides[1].time;
            var time2   = json.tides[2].time;
            var time3   = json.tides[3].time;
            
            var height0 = json.tides[0].height;            
            var height1 = json.tides[1].height;            
            var height2 = json.tides[2].height;
            var height3 = json.tides[3].height;


            
            console.log("0 - state is " + state0 + "time is "+time0 + "height is "+height0);
            console.log("1 - state is " + state1 + "time is "+time1 + "height is "+height1);
            console.log("2 - state is " + state2 + "time is "+time2 + "height is "+height2);
            console.log("3 - state is " + state3 + "time is "+time3 + "height is "+height3);
 
      
            // build dict to send
            var dictionary = {
              "MSG_TYPE": "tides",
              
              "KEY_STATE_0": state0,
              "KEY_TIME_0": time0,
              "KEY_HEIGHT_0": height0,
              
              "KEY_STATE_1": state1,
              "KEY_TIME_1": time1,
              "KEY_HEIGHT_1": height1,
              
              "KEY_STATE_2": state2,
              "KEY_TIME_2": time2,
              "KEY_HEIGHT_2": height2,
              
              "KEY_STATE_3": state3,
              "KEY_TIME_3": time3,
              "KEY_HEIGHT_3": height3
            };
      
            // Send to Pebble
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    console.log("Tide info sent to Pebble successfully");
                 },
                function(e) {
                    console.log("Error sending tides info to Pebble!");
                }
            );
          }      
    );
}



