#include "SolderVersion.h"
#include "minecraft/VersionFilterData.h"
#include <QObject>
#include <modutils.h>

QString SolderVersion::name()
{
	return id;
}

QString SolderVersion::descriptor()
{
	return id;
}

QString SolderVersion::typeString() const
{
	if (is_latest)
		return QObject::tr("Latest");

	if (is_recommended)
		return QObject::tr("Recommended");

	return QString();
}

bool SolderVersion::operator<(BaseVersion &a)
{
	SolderVersion *pa = dynamic_cast<SolderVersion *>(&a);
	if (!pa)
		return true;

	auto & first = id;
	auto & second = pa->id;
	if(first.startsWith('v'))
	{
		first.remove(0,1);
	}
	if(second.startsWith('v'))
	{
		second.remove(0,1);
	}
	Util::Version left(first);
	Util::Version right(second);
	return left < right;
}

bool SolderVersion::operator>(BaseVersion &a)
{
	SolderVersion *pa = dynamic_cast<SolderVersion *>(&a);
	if (!pa)
		return false;

	auto & first = id;
	auto & second = pa->id;
	if(first.startsWith('v'))
	{
		first.remove(0,1);
	}
	if(second.startsWith('v'))
	{
		second.remove(0,1);
	}
	Util::Version left(first);
	Util::Version right(second);
	return left > right;
}

QString SolderVersion::filename()
{
	return QString();
}

QString SolderVersion::url()
{
	return base_url + pack_name + "/" + id;
}

SolderVersionPtr SolderVersion::fromJson(QJsonObject version)
{
	auto versionObj = std::make_shared<SolderVersion>();
	versionObj->base_url = version.value("base_url").toString();
	versionObj->pack_name = version.value("pack_name").toString();
	versionObj->id = version.value("version").toString();
	return versionObj;
}

QJsonObject SolderVersion::toJson()
{
	QJsonObject obj;
	obj.insert("base_url", base_url);
	obj.insert("pack_name", pack_name);
	obj.insert("version", id);
	return obj;
}
