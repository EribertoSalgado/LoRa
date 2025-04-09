function decodeUplink(input) {
  var data = {};

  // Convert bytes to a string
  var payload = String.fromCharCode.apply(null, input.bytes);

  // Parse the voltage from the string if it matches the "voltage=xxx" format
  if (payload.startsWith("voltage=")) {
    var voltStr = payload.substring(8);  // Extract string after "voltage="
    data.voltage = parseFloat(voltStr);  // Convert to float
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
