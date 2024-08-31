// #include <WiFi.h>
// #include <WebServer.h>

// // Replace with your network credentials
// const char* ssid = "MANTRA 9531";
// const char* password = "123456789";

// // Create an instance of the web server on port 80
// WebServer server(80);

// void handleLocationData() {
//     // Check if both latitude and longitude are present in the HTTP request
//     if (server.hasArg("latitude") && server.hasArg("longitude")) {
//         String latitude = server.arg("latitude");   // Get the latitude argument
//         String longitude = server.arg("longitude"); // Get the longitude argument

//         // Print the received location to the Serial Monitor
//         Serial.println("Received location:");
//         Serial.print("Latitude: ");
//         Serial.println(latitude);
//         Serial.print("Longitude: ");
//         Serial.println(longitude);

//         // Send a response back to the client
//         server.send(200, "text/plain", "Location received");
//     } else {
//         // If the necessary arguments are not present, send an error response
//         server.send(400, "text/plain", "Invalid request");
//     }
// }

// void setup() {
//     Serial.begin(115200); // Start the Serial communication at a baud rate of 115200

//     // Connect to the Wi-Fi network
//     WiFi.begin(ssid, password);
//     Serial.print("Connecting to Wi-Fi");
//     while (WiFi.status() != WL_CONNECTED) {
//         delay(1000);
//         Serial.print(".");
//     }
//     Serial.println();
//     Serial.println("Connected to Wi-Fi");

//     // Print the IP address of the ESP32
//     Serial.println("IP address: ");
//     Serial.println(WiFi.localIP());

//     // Define the location endpoint to handle location data
//     server.on("/location", handleLocationData);

//     // Start the HTTP server
//     server.begin();
//     Serial.println("HTTP server started");
// }

// void loop() {
//     // Continuously handle incoming client requests
//     server.handleClient();
// }

#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>

// Replace these with your network credentials
const char* ssid = "MANTRA 9531";
const char* password = "123456789";

WebServer server(80);

// Coordinates of the ESP32 (initialized with default values)
float esp32_latitude = 0.0;
float esp32_longitude = 0.0;

// Coordinates of the device sending live location
float device_latitude = 0.0;
float device_longitude = 0.0;

// Function to calculate the distance between two coordinates
float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    const float R = 6371e3; // Radius of Earth in meters
    float phi1 = lat1 * PI / 180;
    float phi2 = lat2 * PI / 180;
    float deltaPhi = (lat2 - lat1) * PI / 180;
    float deltaLambda = (lon2 - lon1) * PI / 180;

    float a = sin(deltaPhi / 2) * sin(deltaPhi / 2) +
              cos(phi1) * cos(phi2) * sin(deltaLambda / 2) * sin(deltaLambda / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));

    float distance = R * c; // Distance in meters
    return distance;
}

// Function to calculate direction (bearing) from ESP32 to the device
float calculateBearing(float lat1, float lon1, float lat2, float lon2) {
    float phi1 = lat1 * PI / 180;
    float phi2 = lat2 * PI / 180;
    float deltaLambda = (lon2 - lon1) * PI / 180;

    float y = sin(deltaLambda) * cos(phi2);
    float x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(deltaLambda);
    float theta = atan2(y, x);
    float bearing = (theta * 180 / PI + 360); // Bearing in degrees
    bearing = fmod(bearing, 360); // Normalize to 0-360 degrees
    return bearing;
}

// Handler to capture the incoming location data
void handleLocation() {
    if (server.hasArg("latitude") && server.hasArg("longitude")) {
        device_latitude = server.arg("latitude").toFloat();
        device_longitude = server.arg("longitude").toFloat();

        // Calculate distance and bearing
        float distance = calculateDistance(esp32_latitude, esp32_longitude, device_latitude, device_longitude);
        float bearing = calculateBearing(esp32_latitude, esp32_longitude, device_latitude, device_longitude);

        // Print distance and bearing to Serial Monitor
        Serial.printf("Distance to device: %.2f meters\n", distance);
        Serial.printf("Direction to device: %.2f degrees\n", bearing);

        // Respond to the client
        server.send(200, "text/plain", "Location received");
    } else {
        server.send(400, "text/plain", "Bad Request");
    }
}

// Function to set the coordinates of ESP32
void setCoordinates() {
    Serial.println("Enter ESP32's own coordinates:");
    Serial.print("Latitude: ");
    while (!Serial.available()); // Wait for user input
    esp32_latitude = Serial.parseFloat();
    Serial.print(esp32_latitude); // Echo input

    Serial.print("\nLongitude: ");
    while (!Serial.available()); // Wait for user input
    esp32_longitude = Serial.parseFloat();
    Serial.println(esp32_longitude); // Echo input

    Serial.println("Coordinates set successfully.");
}

void setup() {
    Serial.begin(115200);

    // Set ESP32 coordinates by user input
    setCoordinates();

    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/location", handleLocation);
    server.begin();
    Serial.println("Server started");
}

void loop() {
    server.handleClient();
}
