<!DOCTYPE html>
<html>
<head><title>LoRa IoT Project Page</title></head>
<style>
<style>
    body {
        background-image: url('https://st.depositphotos.com/1020422/2306/i/950/depositphotos_23068524-stock-photo-graph-paper-background.jpg');
        background-size: cover;
    }
    table, th, td {
        border: 2px solid black;
        border-radius: 7px;
        color: black;
        padding: 5px;
        border-spacing: 6px;
        margin: auto; /* Center the table */
    }
    th, td {
        text-align: center;
    }
    tr:nth-child(even) {
        background-color: #196b80;
    }
    tr:nth-child(odd) {
        background-color: #c5dde3;
    }
    h1 {
        text-align: center;
        font-size: 35px;
    }
    #chart-container {
        width: 80%;
        margin: auto;
    }
    #myChart {
        border: 2px solid black; /* Border for the canvas */
        border-radius: 7px; /* Optional: rounded corners */
    }
</style>

</style>
<body background='https://st.depositphotos.com/1020422/2306/i/950/depositphotos_23068524-stock-photo-graph-paper-background.jpg'>
<center style='font-size:35px'>
	<b>Welcome to your IoT Page</b>
</center>
<center>
<h1>
<?php
/////////////////////////////////////////////////////////////////////////////////////////////////
    	echo"<table><caption style='border:solid black; background-color:white'>Database Collection for 'node-1'</caption>";
   	 
    	echo "
        	<tr>
            	<th>Node:</th>
            	<th>Time:</th>
            	<th>Light:</th>
        	</tr>
    	";
/////////////////////////////////////////////////////////////////////////////////////////////////
    	$servername = "localhost";
    	$username = "yourDataBaseUsername";
    	$password = "yourDataBasePassword";
    	$dBName = "yourDataBaseName";
    	//
    	// Create dB connection
    	$conn = new mysqli($servername, $username, $password, $dBName);
    	
    	if ($conn->connect_error) {
        	die("Connection failed: sensor_data" . $conn->connect_error);
    	}
   	 
    	$sql = "SELECT * FROM sensor_data WHERE node_name = 'node-1';
";
    	$result = $conn->query($sql);

    	$chart_data="";
   	 
    	$i=0;
        $totalLight = 0;

    	if ($result->num_rows > 0) {
       	 

        	while($row = $result->fetch_assoc()) {
           	 
            	$node[]  = $row['node_name']  ;
   	 
            	$time[] = $row['time_received'];
           	 
            	$light[] = $row['light'];
           	 
/////////////////////////////////////////////////////////////////////////////////////////////////
            	echo"
                	<tr>
                    	<td>$node[$i]</td>
                    	<td>$time[$i]</td>
                    	<td>$light[$i]</td>
                	</tr>";
/////////////////////////////////////////////////////////////////////////////////////////////////
                $totalLight = $light[$i] + $totalLight;
            	$i=$i+1;
        	}
       	 
        	echo"</table>";
    	}
        
        $average = $totalLight / $i;
        
        echo("<p>The average of the light readings is: " . $average . " (V)" );
        echo("</p>");
    
    
	echo "<br>";
?>
</h1>
<h2>
    <?php
// Fetch JSON data from a specified URL
$json_url = "yourWebpage/data.php"; // Replace with your actual JSON data URL
$json_data = file_get_contents($json_url);

// Check if the data was fetched successfully
if ($json_data === FALSE) {
    die("Error fetching data.");
}

// Decode the JSON data into a PHP array
$data = json_decode($json_data, true);

// Check if decoding was successful
if ($data === NULL) {
    die("Error decoding JSON data.");
}

// Prepare labels and values for the chart
$labels = [];
$values = [];

foreach ($data as $item) {
    $labels[] = $item['time_received']; // Assuming 'time_received' is a key in your JSON
    $values[] = $item['light']; // Assuming 'light' is a key in your JSON
}
?>
   <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.6.0/jquery.min.js"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.7.1/chart.min.js"></script>
</head>
<body>
    <div id="chart-container">
        <canvas id="myChart"></canvas>
    </div>

    <script>
        $(document).ready(function() {
            // Prepare data for Chart.js
            var labels = <?php echo json_encode($labels); ?>;
            var values = <?php echo json_encode($values); ?>;

            var ctx = document.getElementById('myChart').getContext('2d');
            var myChart = new Chart(ctx, {
                type: 'bar', // Change to 'line' for a line chart
                data: {
                    labels: labels,
                    datasets: [{
                        label: 'Light Intensity',
                        data: values,
                        backgroundColor: 'rgba(85, 107, 47, 1)',
                        borderColor: 'rgba(0, 128, 0, 1)',
                        borderWidth: 1
                    }]
                },
                options: {
                    scales: {
                        y: {
                            beginAtZero: true
                        }
                    },
                    plugins: {
                        legend: {
                            display: true,
                            position: 'top'
                        }
                    }
                }
            });
        });
    </script>
</h2>
</center>
</body>
</html>