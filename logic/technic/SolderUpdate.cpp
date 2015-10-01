#include "SolderUpdate.h"
#include "SolderInstance.h"
#include "net/NetJob.h"
#include <pathutils.h>
#include <JlCompress.h>
#include <QJsonDocument>

#include "Env.h"

struct SolderModEntry
{
	QString name;
	QString version;
	QString url;
	QString md5;
	QString mcVersion;
	QString filename()
	{
		return name + "-" + version + ".jar";
	}
	QString cacheFile()
	{
		return PathCombine(QLatin1Literal("technic_dl"), mcVersion, filename());
	}
	QString path()
	{
		return PathCombine(ENV.metacache()->getBasePath("cache"), QLatin1Literal("technic_dl"), mcVersion);
	}
	QString filePath()
	{
		return PathCombine(path(), filename());
	}
};


void SolderUpdate::executeTask()
{
	auto status = m_inst->settings()->get("packStatus").toString();
	if(status == "NotInstalled")
	{
		packVersionStart();
	}
	else
	{
		emitSucceeded();
	}
}

void SolderUpdate::versionProgress(int, qint64 min, qint64 max)
{
	double part = double(min)/double(max);
	setProgress(part * 5, 100.0);
}

void SolderUpdate::packProgress(qint64 min, qint64 max)
{
	double part = double(min)/double(max);
	setProgress(5.0 + part * 95.0, 100.0);
}

void SolderUpdate::packVersionStart()
{
	setStatus(tr("Downloading version information"));
	auto version = m_inst->solderVersion();

	auto temp = ByteArrayDownload::make(QUrl(version->url()));
	packVersionDownload = temp;
	connect(packVersionDownload.get(), SIGNAL(succeeded(int)), SLOT(packVersionFinished()));
	connect(packVersionDownload.get(), SIGNAL(failed(int)), SLOT(packVersionFailed()));
	connect(packVersionDownload.get(), SIGNAL(progress(int, qint64, qint64)),
			SLOT(versionProgress(int, qint64, qint64)));
	packVersionDownload->start();
}

void SolderUpdate::packVersionFinished()
{
	ByteArrayDownloadPtr packMeta = std::dynamic_pointer_cast<ByteArrayDownload> (packVersionDownload);
	QString solderFilePath = PathCombine(m_inst->instanceRoot(), "solder.json");
	QFile file(solderFilePath);
	file.open(QIODevice::WriteOnly);
	file.write(packMeta->m_data);
	file.close();
	packStart();
}

void SolderUpdate::packVersionFailed()
{
	emitFailed(tr("Couldn't get pack version..."));
}

void SolderUpdate::packStart()
{
	QString solderFilePath = PathCombine(m_inst->instanceRoot(), "solder.json");
	QFile file(solderFilePath);
	file.open(QIODevice::ReadOnly);
	QJsonDocument dox = QJsonDocument::fromJson(file.readAll());
	file.close();
	auto obj = dox.object();
	auto mcVersion = obj.value("minecraft").toString();
	// FIXME: bullshit.
	m_inst->setIntendedVersionId(mcVersion);
	auto mods = obj.value("mods").toArray();
	for(auto mod: mods)
	{
		auto modObj = mod.toObject();
		auto e = std::make_shared<SolderModEntry>();
		e->name = modObj.value("name").toString();
		e->version = modObj.value("version").toString();
		e->url = modObj.value("url").toString();
		e->md5 = modObj.value("md5").toString();
		e->mcVersion = mcVersion;
		m_mods.append(e);
	}
	qInfo() << "Using minecraft" << mcVersion;

	setStatus(tr("Downloading pack data"));
	packDownload = NetJobPtr(new NetJob("Solder pack packages"));
	for(auto & mod: m_mods)
	{
		auto entry = ENV.metacache()->resolveEntry("cache", mod->cacheFile());
		auto dl = CacheDownload::make(QUrl(mod->url), entry);
		packDownload->addNetAction(dl);
	}
	connect(packDownload.get(), SIGNAL(succeeded()), SLOT(packFinished()));
	connect(packDownload.get(), SIGNAL(failed()), SLOT(packFailed()));
	connect(packDownload.get(), SIGNAL(progress(qint64,qint64)), SLOT(packProgress(qint64,qint64)));
	packDownload->start();
}

void SolderUpdate::packFinished()
{
	setStatus(tr("Extracting packages"));
	for(auto & mod: m_mods)
	{
		auto filename = mod->filePath();
		auto files = JlCompress::extractDir(filename, m_inst->minecraftRoot());
		qInfo() << "Extracted" << filename << files.join(", ");
	}
	m_inst->settings()->set("packStatus", QString("Extracted"));
	emitSucceeded();
}

void SolderUpdate::packFailed()
{
	emitFailed(tr("Couldn't get pack data..."));
}

