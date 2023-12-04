var g1 = new JustGage({ id: 'g1', value: 0, min: 0, max: 50, title: 'Temperature' });
var g2 = new JustGage({ id: 'g2', value: 0, min: 900, max: 1100, title: 'Pressure' });
var g3 = new JustGage({ id: 'g3', value: 0, min: 0, max: 100, title: 'Humidity' });

function updateValues(temperature, pressure, humidity) {
  g1.refresh(temperature);
  g2.refresh(pressure);
  g3.refresh(humidity);
}

setInterval(function() {
  fetch('/data')
    .then(response => response.json())
    .then(data => updateValues(data.temperature, data.pressure, data.humidity));
}, 1000);

