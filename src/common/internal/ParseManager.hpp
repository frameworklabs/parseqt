/*
 * ParseManager.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef PARSEQT__PARSE_MANAGER_HPP_
#define PARSEQT__PARSE_MANAGER_HPP_

#include <QtNetwork/QNetworkAccessManager>
#include <QVariant>

namespace parseqt {

class ParseError;

/// Internal class - use class Parse instead

class ParseManagerDelegate;

class ParseManager {
public:
	/// construction
	ParseManager();
	~ParseManager();
	static ParseManager *instance();

	/// delegate
	ParseManagerDelegate *delegate() const;
	void setDelegate(ParseManagerDelegate *delegate);

	/// configuration
	QString applicationId() const;
	void setApplicationId(const QString &applicationId);
	QString apiKey() const;
	void setApiKey(const QString &apiKey);
	bool trace() const;
	void setTrace(bool trace);

	/// communication
	ParseError *request(QNetworkAccessManager::Operation op, const QString &url, const QVariant& variant,
						QObject *receiver, const char *slot);
	QVariant retrieveJsonReply(QNetworkReply *reply, int expectedStatusCode, ParseError **error);

	/// ifyers
	QVariant jsonify(const QVariant &data, ParseError **error);
	QVariant objectify(const QVariant &json, ParseError **error);

	/// helpers
	static QDateTime dateTimeFromString(const QString &string);
	static QString stringFromDateTime(const QDateTime &dateTime);
	static void debugJson(const QString &message, const QVariant &json);

private:
	ParseManagerDelegate *_delegate;
	QString _applicationId;
	QString _apiKey;
	bool _trace;
	QNetworkAccessManager _accessManager;
};

class ParseManagerDelegate {
public:
	virtual ~ParseManagerDelegate() { }
	virtual QVariant jsonify(const QVariant &data, ParseError **error) = 0;
	virtual QVariant objectify(const QVariant &json, ParseError **error) = 0;
};

} /* namespace parseqt */

#endif /* PARSEQT__PARSE_MANAGER_HPP_ */
