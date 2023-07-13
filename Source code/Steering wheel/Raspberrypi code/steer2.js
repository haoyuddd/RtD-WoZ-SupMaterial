//https://nodejs.org/api/dgram.html
//import dgram from 'dgram'; << change from example code in link above

var dgram = require('dgram')

const server = dgram.createSocket('udp4');

var g = require('logitech-g27')

var options = {
    autocenter: false, // set to false so the wheel will not fight itself when we rotate it
    debug: false,
    range: 900
}

var wheel = {
    currentPos: 0, // initial value does not matter
    moveToPos: 0,  // initial value does not matter
    moved: true
}

server.on('error', (err) => {
  console.log(`server error:\n${err.stack}`);
  server.close();
});

server.on('message', (msg, rinfo) => {
  //console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
  console.log(JSON.parse(msg).angle)
  g.leds('00001') // red led's
  incomming = JSON.parse(msg).angle
  moveToDegree(incomming)
});

server.on('listening', () => {
  const address = server.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

server.bind(41234);
// Prints: server listening 0.0.0.0:41234

function moveToDegree(deg) {
    /*
    @param  {Number}  deg  Degree can be anywhere from 0 (far left) to 450 (center) to 900 (far right).
    */
    deg = deg / options.range * 100

    wheel.moveToPos = deg
    wheel.moved = false

    if (deg < wheel.currentPos) {
        g.forceConstant(0.3)
		//g.leds('1')
    } else {
        g.forceConstant(0.7)
		//g.leds('00001')
    }
}

g.connect(options, function(err) {
    if (err) {
        console.log('Oops -> ' + err)
    }

    g.forceFriction(0.6) // without friction the wheel will tend to overshoot a move command

    moveToDegree(450) //options.range / 2) // center
	//console.log('test' + (options.range / 2))

    g.on('wheel-turn', function(val) {
        wheel.currentPos = val

        if (wheel.moved === false) {
           //console.log('wheel at position ' + val)

            var min = wheel.moveToPos - 1
            var max = wheel.moveToPos + 1

            if (wheel.currentPos >= min && wheel.currentPos <= max) {
                //console.log('--- move complete, turning off force')
                g.forceConstant() // turn off force
				g.leds('111')
                wheel.moved = true
            }
        }
    })
})
