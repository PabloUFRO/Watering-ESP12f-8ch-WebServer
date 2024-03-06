var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
window.addEventListener('load', onLoad);

function onLoad(event) {
  initWebSocket();
}

function initWebSocket() {
  console.log('Trying to open a WebSocket connection...');
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
  console.log('Connection opened');
  websocket.send("states");
}
  
function onClose(event) {
  console.log('Connection closed');
  setTimeout(initWebSocket, 2000);
} 

//The server (ESP) will send a JSON variable with the current valve´s states
function onMessage(event) {
  var myObj = JSON.parse(event.data); //The response comes in JSON format, so we can save the response as a JSON object using the JSON.parse() method
  console.log(myObj);

  for (i in myObj.valves){  //to go through all the valve´s and corresponding states.
    var valve = myObj.valves[i].num;
    var state = myObj.valves[i].state;
    console.log(valve);
    console.log(state);
    
    if (state == "1"){  //If the state is "1", we get the element with num="X" (valve) and set it to checked, to check the checkbox
      document.getElementById(valve).checked = true;
      document.getElementById(valve+"s").innerHTML = "ON";  //Get the element with the num "X" (valve+"s") and update the text to ON
    }
    else{ //A similar process is done when the state is "0".
      document.getElementById(valve).checked = false;
      document.getElementById(valve+"s").innerHTML = "OFF";
    }
  }
  console.log(event.data);
}

//sends a message using the WebSocket connection whenever a switch is toggled on the web page
function toggleCheckbox (element) { //The message contains the valve number we want to control (element.id corresponds to the num of the slider switch that corresponds to the valve number):
  console.log(element.id);
  websocket.send(element.id);
  if (element.checked){
    document.getElementById(element.id+"s").innerHTML = "ON";
  }
  else {
    document.getElementById(element.id+"s").innerHTML = "OFF"; 
  }
}