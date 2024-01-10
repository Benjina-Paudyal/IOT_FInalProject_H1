
// var gateway = `ws://${window.location.hostname}/ws`;
// var websocket;
// // Init web socket when the page loads
// window.addEventListener('load', onload);

// function onload(event) {
//     initWebSocket();
// }

// function getReadings(){
//     websocket.send("getReadings");
// }

// function initWebSocket() {
//     console.log('Trying to open a WebSocket connection…');
//     websocket = new WebSocket(gateway);
//     websocket.onopen = onOpen;
//     websocket.onclose = onClose;
//     websocket.onmessage = onMessage;
// }

// // When websocket is established, call the getReadings() function
// function onOpen(event) {
//     console.log('Connection opened');
//     getReadings();
// }

// function onClose(event) {
//     console.log('Connection closed');
//     setTimeout(initWebSocket, 2000);
// }

// // Function that receives the message from the ESP32 with the readings
// function onMessage(event) {
//     console.log(event.data);
//     var myObj = JSON.parse(event.data);
//     var keys = Object.keys(myObj);

//     for (var i = 0; i < keys.length; i++){
//         var key = keys[i];
//         document.getElementById(key).innerHTML = myObj[key];
//     }
// }



var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var chartT;

// Init web socket when the page loads
window.addEventListener('load', function (event) {
    initWebSocket();
    getReadings();
});

function getReadings() {
    if (websocket.readyState === WebSocket.OPEN) {
        websocket.send("getReadings");
    }
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

// When websocket is established, call the getReadings() function
function onOpen(event) {
    console.log('Connection opened');
    getReadings();

    // Initialize the Highcharts chart 
    chartT = new Highcharts.Chart({
        chart: {
            renderTo: 'chart-temperature'
        },
        series: [
            {
                name: 'Temperature',
                type: 'line',
                color: '#101D42',
                marker: {
                    symbol: 'circle',
                    radius: 3,
                    fillColor: '#101D42',
                }
            }
        ],
        title: {
            text: undefined
        },
        xAxis: {
            type: 'datetime',
            dateTimeLabelFormats: { second: '%H:%M:%S' }
        },
        yAxis: {
            title: {
                text: 'Temperature Celsius Degrees'
            }
        },
        credits: {
            enabled: false
        }
    });
}


function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++) {
        var key = keys[i];
        document.getElementById(key).innerHTML = myObj[key];
    }

    plotTemperature(myObj);
}


// Plot temperature in the temperature chart(changed)
function plotTemperature(jsonValue) {
 
    var x = (new Date()).getTime();
    var y = Number(jsonValue["sensor"]);
  
    if(chartT.series[0].data.length > 40) {
      chartT.series[0].addPoint([x, y], true, true, true);
    } else {
      chartT.series[0].addPoint([x, y], true, false, true);
    }

  }



// Function to get current readings on the webpage when it loads for the first time
function getReadings() {
    var xhr = new XMLHttpRequest();
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            var myObj = JSON.parse(this.responseText);
            console.log(myObj);
            plotTemperature(myObj);
        }
    };
    xhr.open("GET", "/readings", true);
    xhr.send();
}

if (!!window.EventSource) {
    var source = new EventSource('/events');

    source.addEventListener('open', function (e) {
        console.log("Events Connected");
    }, false);

    source.addEventListener('error', function (e) {
        if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
        }
    }, false);

    source.addEventListener('message', function (e) {
        console.log("message", e.data);
    }, false);

    source.addEventListener('new_readings', function (e) {
        console.log("new_readings", e.data);
        var myObj = JSON.parse(e.data);
        console.log(myObj);
        plotTemperature(myObj);
    }, false);
}



