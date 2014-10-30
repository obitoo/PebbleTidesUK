
  //
  //  Init   ======================================
  //

var wait_msg = 0;
var config_open = 0;
var config_url= "http://82.69.65.202:8080/config.html?";

var config_defaults = {
    cfg_invert_col     : "off",
    cfg_show_heights   : "on",
    cfg_line_graph     : "on",
    cfg_port           : "0110"
};

// Listen for when the watchface is opened, then 
// tell Pebble we're good to start receiving messages. 
Pebble.addEventListener('ready',   function(e) {
    console.log("PebbleKit JS ready!");
    
    var dictionary = {
              "MSG_TYPE"        :"ready"};
  
    Pebble.sendAppMessage(dictionary,
                          function(e) {
                            console.log("Ready sent to Pebble successfully");
                          },
                          function(e) {
                            console.log("Error sending Ready to Pebble!");
                          }
                         );
     
    //  getTides("0110");
    }
);



  //
  //  Config page - open ======================================
  //

Pebble.addEventListener("showConfiguration", function() {
  console.log("showing configuration");
  config_open = 1;
  
  // Load up the stored options
  var url = config_url;
  for (var key in config_defaults) {
      var val = localStorage.getItem(key);
      if (val === null || val === 0) { val = config_defaults.getItem(key);
                                       console.log ("Taken config default of "+val+":"+key);}
      url += encodeURIComponent(key) + "=" + encodeURIComponent(val)+"&";
  }
	
	console.log("opening "+url);
  Pebble.openURL(url);
});


//
// config page - closed ======================================
//
Pebble.addEventListener("webviewclosed", function(e) {
  console.log("configuration closed");
  var config = JSON.parse(decodeURIComponent(e.response));
  console.log("Options = " + JSON.stringify(config));
  
  // save to local storage. 
  for (var key in config) {
    localStorage.setItem(key, config[key]);
    console.log("localstorage:"+key+" = "+config[key]);
	}
  
  // Send to Pebble - add MSG_TYPE to allow routing
  var dictionary = {
              "MSG_TYPE"        :"config",
              "CFG_INVERT_COL"  : config.cfg_invert_col,
              "CFG_SHOW_HEIGHTS": config.cfg_show_heights,
              "CFG_LINE_GRAPH"  : config.cfg_line_graph,
              "CFG_PORT"        : config.cfg_port
    
  };
  console.log("Message to Pebble = " + JSON.stringify(dictionary));
  console.log("webviewclosed, wait_msg = " + wait_msg);

  wait_msg = 1;
  Pebble.sendAppMessage(dictionary,
                        function(e) {   //ACK
                          wait_msg = 0;
                          console.log("Config sent to Pebble successfully");
                            config_open = 0;
                          getTides(config.cfg_port);   
                        },
                        function(e) {   //NAK
                          wait_msg = 0;
                          console.log("Error sending Config to Pebble!");
                            config_open = 0;
                          getTides(config.cfg_port);
                        }
                       );

});




  //
  //  Tides   ============================
  //

// Listen for when an AppMessage is received. If its the first one since 
// startup we store the saved config, later we can pass it to the config web 
// page so it reflects Pebbles state correctly
Pebble.addEventListener('appmessage',   function(e) {
    console.log("AppMessage received from Pebble!");
    var location="0110";
    
    // config 
    if (e.payload !== null) {
        console.log("   got payload");
        console.log(JSON.stringify(e.payload));
        console.log("        payload:"+e.payload.CFG_PORT);
      
        // port identifier
        location = e.payload.CFG_PORT;
        // on/off options
        console.log("        CFG_SHOW_HEIGHTS:"+e.payload.CFG_SHOW_HEIGHTS);
        console.log("        CFG_LINE_GRAPH  :"+e.payload.CFG_LINE_GRAPH);
        console.log("        CFG_INVERT_COL  :"+e.payload.CFG_INVERT_COL);
    }

    // make web request for tides
    console.log(" calling getTides - "+location);
    getTides(location);
  }                     
);

var xhrRequest = function (url, type, callback) {
//   console.log("  xhrRequest : "+url);

  var xhr = new XMLHttpRequest();
  xhr.onload = function () { console.log("onload:"); callback(this.responseText);};
  xhr.open(type, url, true);
  xhr.ontimeout = function () { console.log("ontimeout:"); callback(1); };
  xhr.onreadystatechange = function () { console.log("onreadystatechange:"+xhr.readyState);
                                         if (xhr.readyState == 4) callback(this.responseTxt);
                                         if (xhr.readyState == 404) callback(404);};
  
  xhr.send();

};


function getTides(locn) {
  console.log("getTides:"+locn);

  // might work
  if (config_open == 1){
    console.log("Config window open, aborting tides request");
    return;
  }
  
  // Construct URL - TODO - dns
  var url = "http://82.69.65.202:8080/tides"+locn+".json"; //desktop 8080

  // Send request to my Server
  // TODO - what if http get fails? 
  console.log("getTides: about to make request:"+url);
  xhrRequest(url, 'GET', 
          function(responseText) {
            if (responseText == 1){
                console.log("Timeout!");
            }
            if (responseText == 404){
                console.log("404 not found!");
            }
            else {
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
            console.log("getTides, wait_msg = " + wait_msg);
            if (wait_msg === 0) {
            wait_msg = 1;
            Pebble.sendAppMessage(dictionary,
                function(e) {
                    wait_msg = 0;
                    console.log("Tide info sent to Pebble successfully");
                 },
                function(e) {
                    wait_msg = 0;
                    console.log("Error sending tides info to Pebble!");
                }
            );
            }    } }  //else   
    );
}



