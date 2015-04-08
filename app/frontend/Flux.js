.pragma library

// Dispatcher code is based on Facebook's Flux dispatcher implementation

function Dispatcher() {
	this.lastId = 1
	this.callbacks = {}
	this.queue = []
	this.isDispatching = false
	this.isPending = {}
	this.isHandled = {}
	this.payload = null
}

Dispatcher.prototype.register = function(callback) {
	var id = 'ID_' + this.lastId++
	this.callbacks[id] = callback
	return id
}

Dispatcher.prototype.unregister = function(id) {
	delete this.callbacks[id]
}

Dispatcher.prototype.waitFor = function(ids) {
	if (!this.isDispatching) {
		console.error("Dispatcher.waitFor: Called outside of dispatch phase", ids)
	}
	for (var i = 0; i < ids.length; i++) {
		if (this.isPending[ids[i]]) {
			console.error("Dispatcher.waitFor: Circular dependency detected while waiting for " + ids[i])
			continue
		}
		this.invokeCallback(ids[i])
	}
}

Dispatcher.prototype.dispatch = function(payload) {
	this.queue.unshift(payload)
	if (!this.isDispatching) {
		this.run()
	}
}

Dispatcher.prototype.run = function() {
	this.isDispatching = true
	var id
	while (this.queue.length > 0) {
		for (id in this.callbacks) {
			this.isPending[id] = false
			this.isHandled[id] = false
		}
		this.payload = this.queue.pop()
		for (id in this.callbacks) {
			if (this.isPending[id]) {
				continue
			}
			this.invokeCallback(id)
		}
	}
	this.isDispatching = false
}

Dispatcher.prototype.invokeCallback = function(id) {
	this.isPending[id] = true
	this.callbacks[id](this.payload)
	this.isHandled[id] = true
}

var Actions = Object.freeze({
	// Database
	CONNECT_DATABASE: 0,

	// Notes
	SEARCH_NOTE: 1,
	SELECT_NOTE: 2,
	CREATE_NOTE: 3,
	UPDATE_NOTE: 4,
	PERSIST_NOTE: 5,
	DELETE_NOTE: 6,

	// UI related
	OPEN_SEARCH_PANEL: 7,
	CLOSE_SEARCH_PANEL: 8,

	// Tags
	TAG: 9,
	UNTAG: 10
})

// Export single instance of the dispatcher
var dispatcher = new Dispatcher()

// Export Actions

var DatabaseActions = Object.freeze({
	connectDatabase: function(filepath, isOpening) {
		dispatcher.dispatch({
			type: Actions.CONNECT_DATABASE,
			filepath: filepath,
			isOpening: isOpening
		})
	}
})

var NoteActions = Object.freeze({
	search: function(query) {
		dispatcher.dispatch({
			type: Actions.SEARCH_NOTE,
			query: query
		})
	},

	select: function(id) {
		dispatcher.dispatch({
			type: Actions.SELECT_NOTE,
			id: id
		})
	},

	create: function() {
		dispatcher.dispatch({
			type: Actions.CREATE_NOTE
		})
	},

	update: function(content) {
		dispatcher.dispatch({
			type: Actions.UPDATE_NOTE,
			content: content
		})
	},

	persist: function() {
		dispatcher.dispatch({
			type: Actions.PERSIST_NOTE
		})
	},

	deleteNote: function() {
		dispatcher.dispatch({
			type: Actions.DELETE_NOTE
		})
	}
})

var UiActions = Object.freeze({
	openSearchPanel: function() {
		dispatcher.dispatch({ type: Actions.OPEN_SEARCH_PANEL })
	},
	closeSearchPanel: function() {
		dispatcher.dispatch({ type: Actions.CLOSE_SEARCH_PANEL })
	}
})

var TagActions = Object.freeze({
	 tag: function(tagId) {
		 dispatcher.dispatch({ type: Actions.TAG, tagId: tagId })
	 },
	 untag: function(tagId) {
		 dispatcher.dispatch({ type: Actions.UNTAG, tagId: tagId })
	 }
})
