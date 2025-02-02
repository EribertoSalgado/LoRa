<?php
// Setting header to JSON
header('Content-Type: application/json');
header('Access-Control-Allow-Origin: *'); // Allow requests from any origin

// Database credentials
define('DB_HOST', 'localhost');
define('DB_USERNAME', 'yourDataBaseUsername');
define('DB_PASSWORD', 'yourDataBasePassword');
define('DB_NAME', 'yourDataBaseName');

// Get connection
$mysqli = new mysqli(DB_HOST, DB_USERNAME, DB_PASSWORD, DB_NAME);

// Check connection
if ($mysqli->connect_error) {
    // Return error message in JSON format
    echo json_encode(['error' => 'Connection failed: ' . $mysqli->connect_error]);
    exit();
}

// Query to get data from the table
$query = "SELECT node_name, time_received, light FROM sensor_data";

// Execute query
$result = $mysqli->query($query);

// Check if query execution was successful
if (!$result) {
    // Return error message in JSON format
    echo json_encode(['error' => 'Query failed: ' . $mysqli->error]);
    exit();
}

// Loop through the returned data
$data = array();
while ($row = $result->fetch_assoc()) {
    $data[] = $row; // Fetch associative array and add to data array
}

// Close connection
$mysqli->close();

// Print the data as JSON
echo json_encode($data);
?>