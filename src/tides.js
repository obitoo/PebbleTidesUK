/* ---------------------------------------------------------
    Pebble Tides UK
    Copyright (C) 2014 Owen Bullock 
                       owen.skimapp@gmail.com
                       
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ---------------------------------------------------------*/



  //
  //  Init   ======================================
  //
var wait_msg ;
var config_open ;
var config_url;
var config_defaults;
var webserver="http://li646-227.members.linode.com/tides";

// Listen for when the watchface is opened, then 
// tell Pebble we're good to start receiving messages. 
Pebble.addEventListener('ready',   function(e) {
    console.log("--ready event");
    
    wait_msg = 0;
    config_open = 0;
    config_url= webserver+"/config.html?";
    config_defaults = {
       "cfg_invert_col"     : "off",
       "cfg_show_heights"   : "off",
       "cfg_line_graph"     : "off",
       "cfg_port"           : "0110"
    };  
  
    console.log("  config_defaults= " + JSON.stringify(config_defaults));

    var dictionary = {
              "MSG_TYPE"        :"ready"};

  
     Pebble.sendAppMessage(dictionary,
                          function(e) {
                            console.log("  Ready sent to Pebble successfully");
                          },
                          function(e) {
                            console.log("  Error sending Ready to Pebble!");
                          }
                         );
     
    //  getTides("0110");
  console.log("--ready event: exit");

    }
);



  //
  //  Config page - open ======================================
  //

Pebble.addEventListener("showConfiguration", function() {
  
  console.log("--showConfiguration event");

  
  // Load up the stored options
  var url = config_url;
  for (var key in config_defaults) {
      console.log ("  key:"+key);
      var val = localStorage.getItem(key);
      console.log ("  val:"+val);
      if (val === null || val === 0) { 
        console.log("  config_defaults= " + JSON.stringify(config_defaults));

        //val = config_defaults.getItem(key);  // ios issue here I think 
        val = config_defaults[key];  // ios issue here I think 
        console.log ("  1-Taken config default of "+val+":"+key);}
      else {
        console.log ("  1-Taken config stored of "+val+":"+key);
      }
    
      url += encodeURIComponent(key) + "=" + encodeURIComponent(val)+"&";
  }
	
  console.log("  opening config:"+url);
  Pebble.openURL(url);
  config_open = 1; // How do i tell success of prev?
  console.log("  config is open");
});


//
// config page - closed ======================================
//
Pebble.addEventListener("webviewclosed", function(e) {
  console.log("--webviewclosed event (config)");
  
  var config = JSON.parse(decodeURIComponent(e.response));
  console.log("  Options = " + JSON.stringify(config));
  

  // save to local storage. Might be empty though
  for (var key in config) {
    window.localStorage.setItem(key, config[key]);
    console.log("  save config to localstorage:"+key+" = "+config[key]);
	}
  
//     var dictionary = {
//       "MSG_TYPE"        :"config"};

//   // Load up the stored options
//   for (key in config_defaults) {
//       console.log (" DEBUG: config_defaults:"+ JSON.stringify(config_defaults));
//       var val = 0;//localStorage.getItem(key);
    
//       if (val === null || val === 0) { 
//         val = config_defaults.getItem(key);
//         console.log ("2-Taken config default of "+val+":"+key);}
//       else {
//         console.log ("2-Taken config stored of "+val+":"+key);
//       }
    
//       dictionary.add(key, val);
//   }
  
  // Send to Pebble - add MSG_TYPE to allow routing
  var dictionary = {
              "MSG_TYPE"        :"config",
              "CFG_INVERT_COL"  : config.cfg_invert_col,
              "CFG_SHOW_HEIGHTS": config.cfg_show_heights,
              "CFG_LINE_GRAPH"  : config.cfg_line_graph,
              "CFG_PORT"        : config.cfg_port
  };

  console.log("  Message to Pebble = " + JSON.stringify(dictionary));
  console.log("  config webviewclosed, wait_msg = " + wait_msg);

  wait_msg = 1; // not sure if we still need this, i think the webview was the issue not the messaging
  Pebble.sendAppMessage(dictionary,
                        function(e) {   //ACK
                          wait_msg = 0;
                          console.log("  Config sent to Pebble - ACK");
                            config_open = 0;
                          console.log("  config_open= 0, calling getTides()");
                          getTides(window.localStorage.getItem("cfg_port"));   
                          console.log("  getTides() called");
                        },
                        function(e) {   //NAK
                          wait_msg = 0;
                          console.log("  ERROR - sending Config to Pebble - NAK");
                            config_open = 0;
                          console.log("  config_open= 0, calling getTides()");
                          getTides(window.localStorage.getItem("cfg_port"));
                          console.log("  getTides() called");
                        }
                       );
  
  console.log("--webviewclosed event (config) - exit");
});




  //
  //  Tides   ============================
  //

// Listen for when an AppMessage is received. If its the first one since 
// startup we store the saved config, later we can pass it to the config web 
// page so it reflects Pebbles state correctly
Pebble.addEventListener('appmessage',   function(e) {
    console.log("AppMessage received from Pebble!");
    var location="0110"; // failsafe
    
    // config 
    if (e.payload !== null) {
        console.log("   got payload");
        console.log(JSON.stringify(e.payload));
     
        window.localStorage.setItem("cfg_invert_col", e.payload.CFG_INVERT_COL);
        window.localStorage.setItem("cfg_show_heights", e.payload.CFG_SHOW_HEIGHTS);
        window.localStorage.setItem("cfg_line_graph", e.payload.CFG_LINE_GRAPH);
        window.localStorage.setItem("cfg_port", e.payload.CFG_PORT);
      
        location = e.payload.CFG_PORT;
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
  xhr.onreadystatechange = function () { console.log("onreadystatechange:"+xhr.readyState+"   reponseTxt="+this.responseTxt);
                                         if (xhr.readyState == 4) {
                                           if (xhr.status == 404) 
                                             callback(404); 
                                         }};
  
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
  var url = webserver+"/tides"+locn+".json"; //desktop 8080

  // Send request to my Server
  // TODO - what if http get fails? 
  console.log("getTides: about to make request:"+url);
  xhrRequest(url, 'GET', 
          function(responseText) {
            console.log("xhrRequest Callback. responsText = "+responseText);
            var dictionary;
            if (responseText == 1){
                console.log("Timeout!");
                dictionary = {  
                  "MSG_TYPE": "tides",
                  "KEY_STATE_0" : "1",
                  "KEY_TIME_0"  : locn,
                  "KEY_HEIGHT_0": "0.0"
                };
            }
            else if (responseText == 404){
                console.log("404 not found!");
                dictionary = {  
                  "MSG_TYPE": "tides",
                  "KEY_STATE_0" : "404",
                  "KEY_TIME_0"  : locn,
                  "KEY_HEIGHT_0": "0.0"
                };
            }
            else {
            // responseText contains a JSON object with tidetime info
                var json;
                try {
                  json = JSON.parse(responseText);
                } catch(e){
                  console.log(e);
                  return;
                }
          
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
                dictionary = {
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
            } 
            
            // aaand send to Pebble
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
         }}
    );
}



