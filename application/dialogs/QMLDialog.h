#pragma once
#include <QDialog>
#include <QUrl>
#include <QVariant>

class QQuickView;

class QMLDialog: public QDialog
{
	Q_OBJECT
public:
	explicit QMLDialog(QWidget * parent = 0);
	virtual ~QMLDialog() {};

public slots:
	void startDragging();
	void updateDragging();

public:
	QVariant getPropertyValue (const char * name, QVariant defVal);
	void setUrl(QUrl sourceUrl);

protected: /* variables */
    QPoint m_dragPoint;
	QQuickView *m_view = nullptr;
};
