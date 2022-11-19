const mqtt = require('mqtt');

const client = mqtt.connect('mqtt://pi4.pyt.lan');

client.on('connect', function () {
    console.log('Start');
    client.subscribe('esphome/baxislim/diag/res', function (err) {
    })
    let i = 0;

    client.on('message', function (topic, message) {
        msg = message + '';
        if (i > 254) {
            return;
        }
        if (!msg.startsWith('REQ')) {
            i++;
            console.log(msg);
            // console.log(`Request: ${i}`);
            client.publish('esphome/baxislim/diag/cmd', `OT R ${i}`);
        }
    });

    // console.log(`Request: ${i}`);
    client.publish('esphome/baxislim/diag/cmd', `OT R ${i}`);
});
