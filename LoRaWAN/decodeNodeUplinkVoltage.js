function decodeUplink(input) {
  var data = {};

  // Convert bytes to a string
  var payload = String.fromCharCode.apply(null, input.bytes);

  // Parse the temperature from the string if it matches the "temp=xxx" format
  if (payload.startsWith("voltage=")) {
    var voltStr = payload.substring(8);  // Extract the number after "temp="
    data.voltage = parseInt(voltStr);  // Convert the extracted string to an integer
  } else {
    // Handle any unexpected data format
    data.error = "Unexpected payload format: " + payload;
  }

  return {
    data: data,
    warnings: [],
    errors: []
  };
}
