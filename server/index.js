const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');
const { log } = require('console');
const path = require('path');

const app = express();
const port = process.env.PORT || 80;

// Middleware
app.use(cors());
app.use(bodyParser.json());

// Array to store logged GPS data with timestamp
let loggedGPSData = [];

// Function to convert UTC to IST with a space instead of 'T'
function convertUTCtoIST(utcDateString) {
  var dateUTC = new Date(utcDateString);
  var dateUTCinMillis = dateUTC.getTime();

  // Convert UTC to IST
  var dateIST = new Date(dateUTCinMillis);
  dateIST.setHours(dateIST.getHours() + 5);
  dateIST.setMinutes(dateIST.getMinutes() + 30);

  return dateIST.toISOString().replace("T", " ");
}


// Endpoint to display logged GPS data via GET
app.get('/api/gps', (req, res) => {
  // If query parameters are provided, log them
  const latitude = req.query.latitude;
  const longitude = req.query.longitude;

  if (latitude && longitude) {
    const gpsData = { latitude: parseFloat(latitude), longitude: parseFloat(longitude) };

    // Log the GPS data with timestamp in the Indian time zone
    const timestampIST = convertUTCtoIST(new Date().toISOString());
    const loggedData = { timestamp: timestampIST, ...gpsData };
    loggedGPSData.push(loggedData);
    // console.log(loggedGPSData);
    
    console.log(`Received GPS data via GET: ${JSON.stringify(loggedData)}`);

  }

  res.send('Received');
});

// Endpoint to display logged GPS data
app.get('/', (req, res) => {
  const currentTimeIST = convertUTCtoIST(new Date().toISOString()); // Get the current time in the Indian time zone
  res.send(`
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>GPS Logs</title>
        </head>
        <body>
            <h1>Logged GPS Data</h1>
            <p>Current Time (IST): ${currentTimeIST}</p>
            <ul>
                ${loggedGPSData.map(data => `<li>${JSON.stringify(data)}</li>`).join('')}
            </ul>
        </body>
        </html>
    `);
});


app.get('/getCoordinates', (req, res) => {
    const currentTimeIST = convertUTCtoIST(new Date().toISOString()); // Get the current time in the Indian time zone

    let n = loggedGPSData.length;
    let lng = loggedGPSData[n - 1]['longitude'];
    let lat = loggedGPSData[n - 1]['latitude'];

    console.log(lng, lat);

    res.send({ latitude: lat, longitude: lng });
    // Alternatively, you can use an array:
    // res.send([lat, lng]);
});




app.get('/track', (req, res) => {
    const currentTimeIST = convertUTCtoIST(new Date().toISOString()); // Get the current time in the Indian time zone

    let n=loggedGPSData.length;
    let lng= loggedGPSData[n-1]['longitude'], lat=loggedGPSData[n-1]['latitude'];
    console.log(lng,lat);    
    const htmlFilePath = path.join(__dirname, 'public', 'track.html');

    res.sendFile(htmlFilePath)
});


// Start the server
app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`);
});