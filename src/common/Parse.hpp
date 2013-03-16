/*
 * Parse.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef PARSEQT__PARSE_HPP_
#define PARSEQT__PARSE_HPP_

#include <QDateTime>
#include <QMetaType>

namespace parseqt {

class ParseObject;

class Parse : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString applicationId READ applicationId WRITE setApplicationId FINAL)
	Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey FINAL)
	Q_PROPERTY(bool trace READ trace WRITE setTrace FINAL)

public:
	explicit Parse(QObject *parent = 0);
	virtual ~Parse();

	QString applicationId() const;
	void setApplicationId(const QString &applicationId);

	QString apiKey() const;
	void setApiKey(const QString &apiKey);

	bool trace() const;
	void setTrace(bool trace);

public: // factories
	Q_INVOKABLE parseqt::ParseObject *createObject();

public: // quasi-static helpers
	Q_INVOKABLE QDateTime dateTimeFromString(const QString &string);
	Q_INVOKABLE QString stringFromDateTime(const QDateTime &dateTime);

private:
	Q_DISABLE_COPY(Parse)
};

} /* namespace parseqt */

Q_DECLARE_METATYPE(parseqt::Parse *);

#endif /* PARSEQT__PARSE_HPP_ */
