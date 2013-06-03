/*
 * ParseError.hpp
 *
 * Copyright (c) 2013, Framework Labs
 *
 */

#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <bb/cascades/Image>
#include <bb/cascades/ImageView>
#include <QObject>

namespace bb { namespace cascades {
	class Application;
}}

/*!
 * @brief Application pane object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT

public:
    ApplicationUI(bb::cascades::Application *app);
    virtual ~ApplicationUI() {}

    Q_INVOKABLE void setImageFromByteArray(bb::cascades::ImageView *imageView, const QByteArray &data) const;
};

#endif /* ApplicationUI_HPP_ */
