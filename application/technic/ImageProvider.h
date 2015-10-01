#pragma once

#include <QQuickImageProvider>

class ImageProvider : public QObject, public QQuickImageProvider
{
	Q_OBJECT
public:
	ImageProvider();
	virtual ~ImageProvider(){};

public:
	QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
	QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
	QMap<QString, QPixmap> m_pixmaps;
};
