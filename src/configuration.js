Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  var invert = localStorage.getItem('invert');
  if (! invert) invert = false;
  console.log("Invert display: " + invert);

  // Show config page
  console.log('Configuration window opened.');
  Pebble.openURL('http://rgarth.github.io/PebbleP-Time/configuration.html?invert=' + invert);
});

Pebble.addEventListener('webviewclosed',
  function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));

    console.log('Invert: ' + configuration.invert);
    var invert = 0;
    if ( configuration.invert ) {
      invert = 1;
    }
    var dictionary = {
      "KEY_INVERT": invert
    };
    localStorage.setItem('invert', configuration.invert);
    // Send to Pebble
    Pebble.sendAppMessage(dictionary,
      function(e) {
        console.log("Configuration sent to Pebble successfully!");
      },
      function(e) {
        console.log("Error sending configuration info to Pebble!");
      }
    );
  }
);