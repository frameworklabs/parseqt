/*
 * ParseError.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef PARSEQT__PARSE_ERROR_HPP_
#define PARSEQT__PARSE_ERROR_HPP_

#include <QObject>
#include <QMetaType>

namespace parseqt {

class ParseError : public QObject {
	Q_OBJECT
	Q_PROPERTY(Domain domain READ domain CONSTANT FINAL)
	Q_PROPERTY(int code READ code CONSTANT FINAL)
	Q_PROPERTY(QString error READ error CONSTANT FINAL)
	Q_ENUMS(Domain)
	Q_ENUMS(ParseCode)
	Q_ENUMS(JsonCode)

public:
	enum Domain {
		DomainUndefined = 0,
		DomainParse = 1,
		DomainJson = 2,
		DomainPosix = 3,
		DomainQNetwork = 4,
		DomainParseQt = 5
	};

	enum ParseCode {
		ParseCodeInternalServerError = 1,
		ParseCodeConnectionFailed = 100,
		ParseCodeObjectNotFound = 101
	};

	enum JsonCode {
		JsonCodeFailed = 1,
	};

	enum ParseQt {
		ParseQtInternal = 1,
		ParseQtNotInitialized = 2,
		ParseQtInvalidType = 3
	};

	explicit ParseError(QObject *parent = 0);
	ParseError(Domain domain, int code, const QString &error, QObject *parent = 0);
	virtual ~ParseError();

	Domain domain() const { return _domain; }
	int code() const { return _code; }
	QString error() const { return _error; }

private:
	Q_DISABLE_COPY(ParseError)

private:
	Domain _domain;
	int _code;
	QString _error;
};

} /* namespace parseqt */

Q_DECLARE_METATYPE(parseqt::ParseError *);

#endif /* PARSEQT__PARSE_ERROR_HPP_ */
