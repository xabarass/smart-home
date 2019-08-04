// subber.js
const Influx = require('influx')
var zmq = require('zeromq')
  , sock = zmq.socket('sub');

sock.connect('tcp://10.0.0.4:5556');
sock.subscribe('Temperature');
sock.subscribe('Humidity');
console.log('Subscriber connected to port 5556');


const influx = new Influx.InfluxDB({
  host: 'localhost',
  port: 8086,
  database: 'smart_home_test_db',
  schema: [
    {
      measurement: 'Temperature',
      fields: {
        value: Influx.FieldType.FLOAT
      },
      tags: ['room']
    },
    {
      measurement: 'Humidity',
      fields: {
        value: Influx.FieldType.FLOAT
      },
      tags: ['room']
    }
  ]
})


influx.getDatabaseNames()
  .then(names => {
    if (!names.includes('smart_home_test_db')) {
      return influx.createDatabase('smart_home_test_db');
    }
  })
  .then(() => {
	sock.on('message', function(topic, message) {
	      influx.writePoints([
	      {
		measurement: topic.toString('utf-8'),
		tags: { room:'bedroom' },
		fields: { value:parseFloat(message.toString('utf-8'))},
	      }
	    ]).catch(err => {
	      console.error(`Error saving data to InfluxDB! ${err.stack}`)
	    })

	});
  })
  .catch(err => {
    console.error(`Error creating Influx database!`);
  })

