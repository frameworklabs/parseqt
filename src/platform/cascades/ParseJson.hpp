/*
 * ParseJson.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef PARSEQT__PARSE_JSON_HPP_
#define PARSEQT__PARSE_JSON_HPP_

#include <QVariant>

namespace parseqt {

/// Cascades implementation of Json reader/writer

class ParseError;

class ParseJson {
public:
	static QByteArray write(const QVariant &json, ParseError **error);
	static QVariant read(const QByteArray &buffer, ParseError **error);
};

} /* namespace parseqt */

#endif /* PARSEQT__PARSE_JSON_HPP_ */
