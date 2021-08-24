require('dotenv').config()

const mqtt = require('mqtt');
var count = 0;

const settings = {
    port: process.env.NODEJS_MQTT_PORT,
    username: process.env.NODEJS_MQTT_USERNAME,
    password: process.env.NODEJS_MQTT_PASSWD,
    clientId: process.env.NODEJS_MQTT_CLINTID,
}

var client = mqtt.connect(process.env.NODEJS_MQTT_HOST, settings);

console.log("connected flag  " + client.connected);

client.on('connect',()=>{
    // console.log('connected')
    // push connected information
    // client.publish('test','Service online')
    client.subscribe('test')
})

const sensitive = 10;

client.on('message',(topic,msg)=>{
    console.log(`Recieved: ${topic}, Message: ${msg.toString()}`)
})