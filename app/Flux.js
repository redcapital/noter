.pragma library

function Dispatcher() {
	this.lastId = 1;
	this.callbacks = {};
	this.queue = [];
	this.isDispatching = false;
	this.isPending = {};
	this.isHandled = {};
	this.payload = null;
}

Dispatcher.prototype.register = function(callback) {
	var id = 'ID_' + this.lastId++;
	this.callbacks[id] = callback;
	return id;
}

Dispatcher.prototype.unregister = function(id) {
	delete this.callbacks[id];
}

Dispatcher.prototype.waitFor = function(ids) {
	if (!this.isDispatching) {
		console.error("Dispatcher.waitFor: Called outside of dispatch phase", ids);
	}
	for (var i = 0; i < ids.length; i++) {
		if (this.isPending[ids[i]]) {
			console.error("Dispatcher.waitFor: Circular dependency detected while waiting for " + ids[i]);
			continue;
		}
		this.invokeCallback(ids[i]);
	}
}

Dispatcher.prototype.dispatch = function(payload) {
	this.queue.unshift(payload);
	if (!this.isDispatching) {
		this.run();
	}
}

Dispatcher.prototype.run = function() {
	this.isDispatching = true;
	var id;
	while (this.queue.length > 0) {
		for (id in this.callbacks) {
			this.isPending[id] = false;
			this.isHandled[id] = false;
		}
		this.payload = this.queue.pop();
		for (id in this.callbacks) {
			if (this.isPending[id]) {
				continue;
			}
			this.invokeCallback(id);
		}
	}
	this.isDispatching = false;
}

Dispatcher.prototype.invokeCallback = function(id) {
	this.isPending[id] = true;
	this.callbacks[id](this.payload);
	this.isHandled[id] = true;
}
