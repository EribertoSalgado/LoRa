<!DOCTYPE html>
<html>
<head>
    <title>IoT Data Insertion Page</title>
    <style>
        table, tr { background-color: black; }
        table { border-spacing: 6px; }
        table, th, td { border: 2px solid black; color: black; border-radius: 7px; }
        th, td { padding: 5px; }
        tr:nth-child(even) { background-color: #196b80; }
        tr:nth-child(odd) { background-color: #c5dde3; }
    </style>
</head>
<body background='https://st.depositphotos.com/1020422/2306/i/950/depositphotos_23068524-stock-photo-graph-paper-background.jpg'>
    <center style='font-size:35px'>
        <b>IoT Data Push</b>
    </center>
    <center>
        <h1>
        <?php
            $numKeys = 0;
            $light = 0;
            $time = 0;
            $node = '';

            foreach ($_REQUEST as $key => $value) {
                if ($key == "light") {
                    $light = htmlspecialchars($value);
                } else if($key == "time") {
                    $time = htmlspecialchars($value);
                } else if($key == "node") {
                    $node = htmlspecialchars($value);
                }
            }

            // Database credentials
            $servername = "localhost";
    	    $username = "yourDataBaseUsername";
    	    $password = "yourDataBasePassword";
    	    $dBName = "yourDataBaseName";

            // Create database connection
            $conn = new mysqli($servername, $username, $password, $dBName);

            // Check connection
            if ($conn->connect_error) {
                die("Connection failed: " . $conn->connect_error);
            }

            // Insert data into database
            $sql = "INSERT INTO sensor_data (node_name, light, time_received) VALUES ('$node', $light, '$time')"; // This portion must match that of your database - you may or may not be sending light values...
            if ($conn->query($sql) === TRUE) {
                echo "New record created successfully";
            } else {
                echo "Error: " . $sql . "<br>" . $conn->error;
            }

            $conn->close();
        ?>
        </h1>
    </center>
</body>
</html>