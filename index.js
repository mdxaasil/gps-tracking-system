const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');

const app = express();
const port = process.env.PORT || 3000;

// Middleware
app.use(cors());
app.use(bodyParser.json());

// Array to store logged GPS data
const loggedGPSData = [];

// Endpoint to handle incoming GPS data
app.post('/api/gps', (req, res) => {
  const gpsData = req.body;

  // Log the GPS data
  loggedGPSData.push(gpsData);
  console.log(`Received GPS data: ${gpsData}`);
  // Send a response back to the client
  res.status(200).send('RECEIVED');
});

// Endpoint to display logged GPS data via GET
// Endpoint to display logged GPS data via GET
app.get('/api/gps', (req, res) => {
  // If query parameters are provided, log them
  const latitude = req.query.latitude;
  const longitude = req.query.longitude;

  if (latitude && longitude) {
    const gpsData = { latitude: parseFloat(latitude), longitude: parseFloat(longitude) };
    loggedGPSData.push(gpsData);
    console.log(`Received GPS data via GET: ${JSON.stringify(gpsData)}`);

  }

  res.send('Received');
});

// Endpoint to display logged GPS data
app.get('/gps-logs', (req, res) => {
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
            <ul>
                ${loggedGPSData.map(data => `<li>${JSON.stringify(data)}</li>`).join('')}
            </ul>
        </body>
        </html>
    `);
});

// Start the server
app.listen(port, () => {
  console.log(`Server is running on http://localhost:${port}`);
});
