#ifndef ACTIONS_H
#define ACTIONS_H

#include <QObject>

class Actions : public QObject
{
	Q_OBJECT

public:
	enum ActionTypes {
		// Database
		CONNECT_DATABASE,

		// Notes
		SEARCH_NOTE,
		SELECT_NOTE,
		CREATE_NOTE,
		UPDATE_NOTE,
		PERSIST_NOTE,
		DELETE_NOTE
	};

	Q_ENUMS(ActionTypes)
};

#endif // ACTIONS_H
