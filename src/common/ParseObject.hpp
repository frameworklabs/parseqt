/*
 * ParseObject.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef PARSEQT__PARSE_OBJECT_HPP_
#define PARSEQT__PARSE_OBJECT_HPP_

#include <QDateTime>
#include <QtDeclarative/qdeclarativepropertymap.h>

namespace parseqt {

class ParseQuery;
class ParseError;

class ParseObject : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString className READ className WRITE setClassName FINAL)
	Q_PROPERTY(QDeclarativePropertyMap *data READ data NOTIFY dataChanged FINAL)
	Q_PROPERTY(QString objectId READ objectId NOTIFY objectIdChanged FINAL)
	Q_PROPERTY(QDateTime createdAt READ createdAt NOTIFY createdAtChanged FINAL)
	Q_PROPERTY(QDateTime updatedAt READ updatedAt NOTIFY updatedAtChanged FINAL)
	Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)

public:
	/// creating objects
	explicit ParseObject(QObject *parent = 0);
	virtual ~ParseObject();

	/// managing object properties
	QString className() const;
	void setClassName(const QString &className);

	QDeclarativePropertyMap *data();
	QString objectId() const;
	QDateTime createdAt() const;
	QDateTime updatedAt() const;

	bool busy() const;

	/// saving an object
	Q_INVOKABLE void save();
	Q_SIGNAL void saveCompleted(bool succeeded, parseqt::ParseError *error);

	/// deleting objects (delete and destroy are reserved names/functions in C++ and JS)
	Q_INVOKABLE void erase();
	Q_SIGNAL void eraseCompleted(bool succeeded, parseqt::ParseError *error);

Q_SIGNALS:
	void dataChanged();
	void objectIdChanged();
	void createdAtChanged();
	void updatedAtChanged();
	void busyChanged(bool busy);

private:
	friend class ParseQuery;

	ParseError *setData(const QVariantMap &jsonMap);

private:
	Q_DISABLE_COPY(ParseObject)

	QVariant toJson(ParseError **error) const;
	ParseError *fromJsonMap(const QVariantMap &jsonMap);

	void createObject();
	Q_SLOT void createObjectFinished();

	void updateObject();
	Q_SLOT void updateObjectFinished();

	Q_SLOT void eraseFinished();

	void setBusy(bool busy);

private:
	QString _className;
	QDeclarativePropertyMap _data;
	QVariantMap _snapshot;
	bool _busy;
};

} /* namespace parseqt */

Q_DECLARE_METATYPE(parseqt::ParseObject *);

#endif /* PARSEQT__PARSE_OBJECT_HPP_ */
