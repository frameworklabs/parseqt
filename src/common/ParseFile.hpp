/*
 * ParseFile.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef PARSEQT__PARSE_FILE_HPP_
#define PARSEQT__PARSE_FILE_HPP_

#include <QUrl>
#include <QMetaType>

namespace parseqt {

class ParseError;

class ParseFile : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
	Q_PROPERTY(QByteArray data READ data WRITE setData NOTIFY dataChanged FINAL)

	Q_PROPERTY(bool dirty READ dirty NOTIFY dirtyChanged FINAL)
	Q_PROPERTY(bool dataAvailable READ dataAvailable NOTIFY dataAvailableChanged FINAL)
	Q_PROPERTY(QUrl url READ url NOTIFY urlChanged FINAL)

public:
	explicit ParseFile(QObject *parent = 0);
	virtual ~ParseFile();

	/// properties
	QString name() const;
	void setName(const QString &name);
	Q_SIGNAL void nameChanged(const QString &name);

	QByteArray data() const;
	void setData(const QByteArray &data);
	Q_SIGNAL void dataChanged(const QByteArray &data);

	Q_INVOKABLE bool setDataFromFile(const QString &fileName);

	bool dirty() const;
	Q_SIGNAL void dirtyChanged(bool dirty);

	bool dataAvailable() const;
	Q_SIGNAL void dataAvailableChanged(bool dataAvailable);

	QUrl url() const;
	Q_SIGNAL void urlChanged(const QUrl &url);

	/// storing data with Parse
	Q_INVOKABLE void save();
	Q_SIGNAL void saveCompleted(bool succeeded, parseqt::ParseError *error);

	/// getting data from Parse
	Q_INVOKABLE void getData();
	Q_SIGNAL void getDataCompleted(const QByteArray &data, parseqt::ParseError *error);

private:
	Q_DISABLE_COPY(ParseFile)

	Q_SLOT void saveFinished();
	Q_SLOT void getDataFinished();

	void setDirty(bool value);
	void setDataAvailable(bool value);

private:
	QString _name;
	QUrl _url;
	QByteArray _data;
	bool _dirty;
	bool _dataAvailable;
};

} /* namespace parseqt */

Q_DECLARE_METATYPE(parseqt::ParseFile *);

#endif /* PARSEQT__PARSE_FILE_HPP_ */
