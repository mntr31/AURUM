function startFetchingLocation() {
    document.getElementById('status-text').innerText = 'Fetching Location...';

    if (navigator.geolocation) {
        navigator.geolocation.getCurrentPosition(
            function (position) {
                // Successfully retrieved location
                const latitude = position.coords.latitude;
                const longitude = position.coords.longitude;

                document.getElementById('status-text').innerText = 'Location Fetched';
                document.getElementById('latitude').innerText = latitude.toFixed(6);
                document.getElementById('longitude').innerText = longitude.toFixed(6);

                // Try to send location to ESP32
                checkConnectionAndSendLocation(latitude, longitude);
            },
            function (error) {
                // Handle errors
                switch (error.code) {
                    case error.PERMISSION_DENIED:
                        document.getElementById('status-text').innerText = 'Failed to fetch location: Permission Denied';
                        break;
                    case error.POSITION_UNAVAILABLE:
                        document.getElementById('status-text').innerText = 'Failed to fetch location: Position Unavailable';
                        break;
                    case error.TIMEOUT:
                        document.getElementById('status-text').innerText = 'Failed to fetch location: Timeout';
                        break;
                    default:
                        document.getElementById('status-text').innerText = 'Failed to fetch location: Unknown Error';
                        break;
                }
                console.error("Error fetching location:", error);
            }
        );
    } else {
        document.getElementById('status-text').innerText = 'Geolocation Not Supported';
    }
}

function checkConnectionAndSendLocation(lat, lon) {
    fetch('http://192.168.1.100/check-connection')  // Adjust with your ESP32 IP and endpoint
    .then(response => response.json())
    .then(data => {
        if (data.connected) {
            document.getElementById('connection-status').innerText = 'Connected';
            sendLocationToESP32(lat, lon);
        } else {
            document.getElementById('connection-status').innerText = 'Not Connected';
        }
    })
    .catch(error => {
        console.error("Error checking connection:", error);
        document.getElementById('connection-status').innerText = 'Not Connected';
    });
}

function sendLocationToESP32(lat, lon) {
    fetch('http://192.168.1.100/location', {  // Replace with your ESP32 IP address
        method: 'POST',
        headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
        },
        body: `latitude=${lat}&longitude=${lon}`
    })
    .then(response => response.text())
    .then(data => {
        console.log("Location sent successfully:", data);
        document.getElementById('status-text').innerText = 'Location Sent';
    })
    .catch(error => {
        console.error("Failed to send location:", error);
        document.getElementById('status-text').innerText = 'Error Sending Location';
    });
}
