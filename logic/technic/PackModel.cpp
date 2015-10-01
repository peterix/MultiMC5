#include "PackModel.h"
#include <QUrl>
#include <QStringListModel>
#include <QPixmap>
#include <QDebug>
#include "Json.h"
#include "net/ByteArrayDownload.h"

/*
	"tekkitmain":
	{
		"name": "tekkitmain",
		"display_name": "Tekkit",
		"url": "http:\/\/www.technicpack.net\/tekkit",
		"icon": "http:\/\/cdn.technicpack.net\/resources\/tekkitmain\/icon.png?1410213214",
		"icon_md5": "1dd87c03268a7144411bb8cbe8bf7326",
		"logo": "http:\/\/cdn.technicpack.net\/resources\/tekkitmain\/logo.png?1410213214",
		"logo_md5": "2f9625f8343cd1aaa35d3dd631ad64e1",
		"background":
   "http:\/\/cdn.technicpack.net\/resources\/tekkitmain\/background.png?1410213214",
		"background_md5": "f39ae618809383451f6832e4d2a738fe",
		"recommended": "1.2.9e",
		"latest": "1.2.10c",
		"builds": [
			"1.0.2",
			"1.0.3",
			...
		]
	},
*/

std::shared_ptr<SolderPackInfo> loadSolderPackInfo(QJsonObject object)
{
	using namespace Json;
	auto packInfo = std::make_shared<SolderPackInfo>();
	try
	{
		auto remoteImage = [&packInfo, &object](QString ident) -> QString
		{
			QString value = object.value(ident).toString();
			if (value.isEmpty())
			{
				return value;
			}
			QByteArray ba;
			ba.append(value);
			auto result =
				QString("image://url/%1/%2$%3").arg(packInfo->name, ident, ba.toBase64());
			QString md5 = object.value(ident + "_md5").toString();
			if (!md5.isEmpty())
			{
				result.append('$');
				result.append(md5);
			}
			return result;
		};

		// TODO: more than one repo/pack subscription...
		packInfo->repo = "http://solder.technicpack.net/api/modpack/";
		packInfo->name = ensureString(object.value("name"), "name");
		packInfo->display_name = ensureString(object.value("display_name"), "display_name");
		packInfo->url = object.value("url").toString();
		packInfo->icon = remoteImage("icon");
		packInfo->logo = remoteImage("logo");
		packInfo->background = remoteImage("background");

		QString recommended = ensureString(object.value("recommended"), "recommended");
		QString latest = ensureString(object.value("latest"), "latest");
		QStringList builds = ensureIsArrayOf<QString>(object.value("builds"), "builds");
		QSet<SolderVersion> buildSet;
		for (auto build : builds)
		{
			auto v = std::make_shared<SolderVersion>();
			v->base_url = packInfo->repo;
			v->pack_name = packInfo->name;
			v->id = build;
			v->is_complete = false;
			v->is_latest = latest == build;
			v->is_recommended = recommended == build;
			packInfo->builds.append(v);
		}
		std::sort(packInfo->builds.begin(), packInfo->builds.end(),
				  [](const SolderVersionPtr &left, const SolderVersionPtr &right)
				  { return (*left) > (*right); });
		int index = 0;
		for(auto build: packInfo->builds)
		{
			if(build->id == recommended)
			{
				packInfo->recommended = index;
			}
			if(build->id == latest)
			{
				packInfo->latest = index;
			}
			index++;
		}
	}
	catch (JSONValidationError &e)
	{
		qWarning() << "Error parsing Solder pack: " << e.cause();
		return nullptr;
	}
	return packInfo;
}

PackModel::PackModel(QObject *parent) : QMLAbstractListModel(parent)
{
	populate();
}

void PackModel::populate()
{
	QString source("http://solder.technicpack.net/api/modpack/?include=full");

	m_dlAction = ByteArrayDownload::make(QUrl(source));
	connect(m_dlAction.get(), SIGNAL(succeeded(int)), SLOT(dataAvailable()));
	m_dlAction->start();
}

SolderPackInfoPtr PackModel::packByIndex(int index)
{
	if (index < 0 || index >= rowCount())
		return nullptr;

	return m_packs[index];
}

void PackModel::dataAvailable()
{
	beginResetModel();
	m_packs.clear();
	auto document = QJsonDocument::fromJson(m_dlAction->m_data);
	if (document.isNull())
	{
		qWarning() << m_dlAction->m_data;
		qWarning() << "Got gibberish from Technic instead of a pack list";
		return;
	}
	auto modpacksValue = document.object().value("modpacks");
	if (modpacksValue.isNull())
	{
		qWarning() << "No modpacks in the retrieved json";
		return;
	}
	auto modpacksObject = modpacksValue.toObject();
	auto iter = modpacksObject.begin();
	while (iter != modpacksObject.end())
	{
		QString packName = iter.key();
		auto packValue = iter.value();
		if (!packValue.isObject())
		{
			qWarning() << "Pack" << packName << "is not an object.";
			iter++;
			continue;
		}
		auto pack = loadSolderPackInfo(packValue.toObject());
		if (!pack)
		{
			qWarning() << "Pack" << packName << "could not be loaded.";
			iter++;
			continue;
		}
		if (pack->name == "vanilla")
		{
			qWarning() << "Pack" << packName << "ignored.";
			iter++;
			continue;
		}
		m_packs.push_back(pack);
		iter++;
	}
	endResetModel();
}

QHash<int, QByteArray> PackModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[NameRole] = "name";
	roles[DisplayNameRole] = "display_name";
	roles[LogoRole] = "logo";
	roles[BackgroundRole] = "background";
	roles[RecommendedRole] = "recommended";
	roles[LatestRole] = "latest";
	return roles;
}

QVariant PackModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
		return QVariant();

	auto pack = m_packs[index.row()];
	switch (role)
	{
	case Qt::DisplayRole:
	case NameRole:
		return pack->name;
	case DisplayNameRole:
		return pack->display_name;
	case LogoRole:
		return pack->logo;
	case BackgroundRole:
		return pack->background;
	case RecommendedRole:
		return pack->recommended;
	case LatestRole:
		return pack->latest;
	default:
		return QVariant();
	}
}

QVariantMap QMLAbstractListModel::get(int row)
{
	QHash<int, QByteArray> names = roleNames();
	QHashIterator<int, QByteArray> i(names);
	QVariantMap res;
	while (i.hasNext())
	{
		i.next();
		QModelIndex idx = index(row, 0);
		QVariant data = idx.data(i.key());
		res[i.value()] = data;
	}
	return res;
}

int PackModel::rowCount(const QModelIndex &) const
{
	return m_packs.size();
}

QHash<int, QByteArray> VersionModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[NameRole] = "name";
	roles[RecommendedRole] = "recommended";
	roles[LatestRole] = "latest";
	return roles;
}

QVariant VersionModel::data(const QModelIndex &index, int role) const
{
	if (!m_base)
	{
		return QVariant();
	}
	auto builds = m_base->builds;

	if (!index.isValid() || index.row() < 0 || index.row() >= builds.size())
		return QVariant();

	auto row = index.row();
	auto build = builds[row];
	switch (role)
	{
	case Qt::DisplayRole:
	case NameRole:
		return build->id;
	case LatestRole:
		return build->is_latest;
	case RecommendedRole:
		return build->is_recommended;
	default:
		return QVariant();
	}
}

int VersionModel::rowCount(const QModelIndex &) const
{
	if (!m_base)
	{
		return 0;
	}
	return m_base->builds.size();
}

SolderVersionPtr VersionModel::versionById(QString id)
{
	auto builds = m_base->builds;
	auto predicate = [&id](SolderVersionPtr item)
	{
		return item->id == id;
	};
	auto build = std::find_if(builds.begin(), builds.end(), predicate);
	if(build != builds.end())
	{
		return *build;
	}
	return nullptr;
}
