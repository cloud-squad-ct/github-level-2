var assert = require('assert')
const { sayHello } = require('../lib')

function testWho(who) {
	re = new RegExp(`Hello from ${who}!`)
	assert.match(sayHello(who), re)
}

function testGreen() {
	assert.equal(parseInt(sayHello('test').substring(2, 4)), 32)
}

testWho('foo')
testGreen()
