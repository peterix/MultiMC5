#pragma once
#include <QAbstractListModel>
#include <memory>
#include "SolderVersion.h"

#include "multimc_logic_export.h"

class ByteArrayDownload;

struct SolderPackInfo
{
	QString repo;
	QString name;
	QString display_name;
	QString url;
	QString icon;
	QString logo;
	QString background;
	int recommended = -1;
	int latest = -1;
	QList<SolderVersionPtr> builds;
};
typedef std::shared_ptr<SolderPackInfo> SolderPackInfoPtr;

class MULTIMC_LOGIC_EXPORT QMLAbstractListModel : public QAbstractListModel
{
	Q_OBJECT
public:
	QMLAbstractListModel(QObject* parent):QAbstractListModel(parent){};
	Q_INVOKABLE QVariantMap get(int row);
};

class MULTIMC_LOGIC_EXPORT VersionModel : public QMLAbstractListModel
{
	Q_OBJECT
	struct Item;

public:
	enum Roles
	{
		NameRole = Qt::UserRole + 1,
		RecommendedRole,
		LatestRole
	};

public:
	VersionModel(SolderPackInfoPtr base, QObject *parent = 0) :QMLAbstractListModel(parent)
	{
		reset(base);
	}
	void reset (SolderPackInfoPtr base)
	{
		beginResetModel();
		m_base = base;
		endResetModel();
	}
	SolderVersionPtr versionById(QString id);

public: /* model interface */
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QHash<int, QByteArray> roleNames() const;

private:
	SolderPackInfoPtr m_base;
};

class MULTIMC_LOGIC_EXPORT PackModel : public QMLAbstractListModel
{
	Q_OBJECT
public:
	enum Roles
	{
		NameRole = Qt::UserRole + 1,
		DisplayNameRole,
		LogoRole,
		BackgroundRole,
		VersionModelRole,
		RecommendedRole,
		LatestRole
	};

public:
	PackModel(QObject *parent);

public:
	void populate();
	SolderPackInfoPtr packByIndex(int index);

private slots:
	void dataAvailable();

public: /* model interface */
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QHash<int, QByteArray> roleNames() const;

protected:
	QList<SolderPackInfoPtr> m_packs;
	std::shared_ptr<ByteArrayDownload> m_dlAction;
};
