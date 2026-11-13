var colors = require('colors')

function sayHello(who) {
	msg = `Hello from ${who}!`
	return msg.green
}

module.exports = {
	sayHello
}
