/*
 * ParseQuery.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef PARSEQT__PARSE_QUERY_HPP_
#define PARSEQT__PARSE_QUERY_HPP_

#include <QVariant>
#include <QMetaType>

namespace parseqt {

class ParseObject;
class ParseError;

class ParseQuery : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString className READ className WRITE setClassName FINAL)
	Q_PROPERTY(int limit READ limit WRITE setLimit FINAL)
	Q_PROPERTY(int skip READ skip WRITE setSkip FINAL)
	Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)

public:
	/// creating a query
	explicit ParseQuery(QObject *parent = 0);
	virtual ~ParseQuery();

	/// managing object properties
	QString className() const;
	void setClassName(const QString &className);

	bool busy() const;
	Q_SIGNAL void busyChanged(bool busy);

	/// getting objects by id
	Q_INVOKABLE void getObjectById(const QString &objectId);
	Q_SIGNAL void getObjectByIdCompleted(parseqt::ParseObject *object, ParseError *error);

	/// adding basic constraints
	Q_INVOKABLE void whereLessThan(const QString &key, const QVariant &what);
	Q_INVOKABLE void whereLessThanOrEqualTo(const QString &key, const QVariant &what);
	Q_INVOKABLE void whereGreaterThan(const QString &key, const QVariant &what);
	Q_INVOKABLE void whereGreaterThanOrEqualTo(const QString &key, const QVariant &what);
	Q_INVOKABLE void whereNotEqualTo(const QString &key, const QVariant &what);

	/// sorting
	Q_INVOKABLE void orderByAscending(const QString &key);
	Q_INVOKABLE void addAscendingOrder(const QString &key);
	Q_INVOKABLE void orderByDescending(const QString &key);
	Q_INVOKABLE void addDescendingOrder(const QString &key);

	/// pagination
	int limit() const;
	void setLimit(int limit);
	int skip() const;
	void setSkip(int skip);

	/// finding objects as specified
	Q_INVOKABLE void findObjects();
	Q_SIGNAL void findObjectsCompleted(const QVariant &results, parseqt::ParseError *error);

private:
	Q_DISABLE_COPY(ParseQuery)

	Q_SLOT void getObjectByIdFinished();
	Q_SLOT void findObjectsFinished();

	void where(const QString &op, const QString &key, const QVariant &what);
	void addOrder(const QString &key, Qt::SortOrder sortOrder);

	QVariant constraints(ParseError **error);

	void setBusy(bool busy);

private:
	QString _className;
	QVariantMap _where;
	QVariantList _order;
	int _limit;
	int _skip;
	bool _busy;
};

} /* namespace parseqt */

Q_DECLARE_METATYPE(parseqt::ParseQuery *);

#endif /* PARSEQT__PARSE_QUERY_HPP_ */
