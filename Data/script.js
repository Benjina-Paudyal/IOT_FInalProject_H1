

// Code adapted from https://randomnerdtutorials.com/
// Consulted ChatGPT for guidance on handling edge cases


var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// init web socket when page loads
window.addEventListener('load', onload);


function onload(event) {
    initWebSocket();
}

function getReadings(){
    websocket.send("getReadings");
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
}

function onClose(event) {
    console.log('Connection closed',event);
    setTimeout(initWebSocket, 2000);
}


// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
    console.log(event.data);
    var myObj = JSON.parse(event.data);
    var keys = Object.keys(myObj);

    for (var i = 0; i < keys.length; i++){
        var key = keys[i];
        document.getElementById("temperature").innerHTML = myObj[key];
    }
    plotTemperature(myObj)
}
    // Initialize the Highcharts chart 
    var chartOptions = {
        chart:{
          renderTo:'chart-temperature'
        },
        series: [
          {
            // Specifications for the grap visuals
            name: 'Temperature C',
            type: 'spline',
            color: '#2FCDE4',
            fillColor: '#D8F7FB',
            marker: {
              symbol: 'circle',
              radius: 3,
              fillColor: '#27285D',
            }
          }
        ],
        title: {
          text: undefined
        },
        xAxis: {
          type: 'datetime',
          dateTimeLabelFormats: { second: '%Y-%m-%d %H:%M:%S' }
        },
        yAxis: {
          title: {
            text: 'Temperature Celsius Degrees'
          }
        },
        credits: {
           enabled: false
        }
      };

    var chartT=new Highcharts.Chart('chart-temperature',chartOptions);
    // Removes current graph and rebuilds it
    function clearGraph(){
        console.log("Clear graph")
        chartT.series[0].setData([], true, true, true);
        // chartT.destroy();
        // chartT = new Highcharts.Chart(chartOptions);
        getReadings();
      }
      
      // // Deletes content of temperature.csv
      // function deleteFile(){
      //   console.log("Delete content of CSV file");
      //   fetch("/data.csv",{method: "DELETE"}).then((res) => console.log(res.text()));
      // }

      // Deletes content of temperature.csv
function deleteFile() {
  console.log("Delete content of CSV file");

  // Get the element where you want to display the result message
  var resultMessageElement = document.getElementById("result-message");

  // Clear any previous messages
  resultMessageElement.innerText = "";

  fetch("/data.csv", { method: "DELETE" })
      .then((res) => {
          // Check if the deletion was successful
          if (res.ok) {
              // Display a success message
              resultMessageElement.innerText = "Data deleted successfully!";
          } else {
              // Display an error message
              resultMessageElement.innerText = "Failed to delete data.";
          }
      })
      .catch((error) => {
          // Display an error message
          console.error("Error deleting data:", error);
          resultMessageElement.innerText = "Error deleting data.";
      });
}


      function downloadCSV() {
        var link = document.createElement("a");
        link.href = "/data.csv";
        link.download = "temprature_data.csv";
        link.click();
      }
    
      
    //Plot temperature in the temperature chart
    function plotTemperature(jsonValue) {
    
    var keys = Object.keys(jsonValue);
    console.log(keys);
    console.log(keys.length);
    
    for (var i = 0; i < keys.length; i++){
      var x = (new Date()).getTime();
      console.log(x);
      const key = keys[i];
      var y = Number(jsonValue[key]);
      console.log(y);
    
      if(chartT.series[i].data.length > 40) {
        chartT.series[i].addPoint([x, y], true, true, true);
      } else {
        chartT.series[i].addPoint([x, y], true, false, true);
      }
    
    }
    }
    
    
    
    
    
    
    
    











