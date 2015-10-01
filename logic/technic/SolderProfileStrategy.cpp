#include "SolderProfileStrategy.h"
#include "minecraft/MinecraftProfile.h"
#include "minecraft/VersionBuildError.h"
#include "minecraft/MinecraftVersion.h"
#include "technic/SolderInstance.h"
#include <JlCompress.h>
#include <pathutils.h>
#include <QJsonDocument>
#include <QRegularExpression>

SolderProfileStrategy::SolderProfileStrategy(SolderInstance* instance)
	: OneSixProfileStrategy(instance)
{

}

bool SolderProfileStrategy::installJarMods(QStringList filepaths)
{
	return false;
}

VersionFilePtr processForgeJsonForTechnic(VersionFilePtr patch, MinecraftVersionPtr vanillaVersion)
{
	auto output = std::make_shared<VersionFile>();
	output->order = 5;
	int sliding_insert_window = 0;

	auto vanillaPatch = vanillaVersion->getVersionFile();

	// A blacklist
	QSet<QString> blacklist{"authlib", "realms"};
	//
	QList<QString> xzlist{"org.scala-lang", "com.typesafe"};
	// for each library in the version we are adding (except for the blacklisted)
	for (auto lib : patch->overwriteLibs)
	{
		QString libName = lib->artifactId();
		QString rawName = lib->rawName();

		// ignore other blacklisted (realms, authlib)
		if (blacklist.contains(libName))
			continue;

		if(libName == "forge" || libName == "minecraftforge")
		{
			continue;
		}

		// WARNING: This could actually break.
		// mark bad libraries based on the xzlist above
		for (auto entry : xzlist)
		{
			qDebug() << "Testing " << rawName << " : " << entry;
			if (rawName.startsWith(entry))
			{
				lib->setHint("forge-pack-xz");
				break;
			}
		}

		QJsonObject libObj = lib->toJson();

		bool found = false;
		bool equals = false;
		// find an entry that matches this one
		for (auto tolib : vanillaPatch->overwriteLibs)
		{
			if (tolib->artifactId() != libName)
				continue;
			found = true;
			if (tolib->toJson() == libObj)
			{
				equals = true;
			}
			// replace lib
			lib->insertType = RawLibrary::Replace;
			break;
		}
		if (equals)
		{
			continue;
		}
		if (!found)
		{
			// add lib
			lib->insertType = RawLibrary::Prepend;
			sliding_insert_window++;
		}
		output->addLibs.prepend(lib);
	}
	output->mainClass = patch->mainClass;
	QString args = patch->overwriteMinecraftArguments;
	QStringList tweakers;
	{
		QRegularExpression expression("--tweakClass ([a-zA-Z0-9\\.]*)");
		QRegularExpressionMatch match = expression.match(args);
		while (match.hasMatch())
		{
			tweakers.append(match.captured(1));
			args.remove(match.capturedStart(), match.capturedLength());
			match = expression.match(args);
		}
	}
	if (!args.isEmpty() && args != vanillaPatch->overwriteMinecraftArguments)
	{
		output->overwriteMinecraftArguments = args;
	}
	if (!tweakers.isEmpty())
	{
		output->addTweakers = tweakers;
	}
	if (!patch->processArguments.isEmpty() &&
		patch->processArguments != vanillaPatch->processArguments)
	{
		output->processArguments = patch->processArguments;
	}

	output->name = "Technic Pack";
	output->fileId = "net.technicpack";
	output->version = "1.0.0";
	output->mcVersion = vanillaPatch->mcVersion;
	ProfileUtils::removeLwjglFromPatch(output);
	qDebug() << output->toJson(false).toJson();
	return output;
}

void SolderProfileStrategy::loadModpackJarPatch()
{
	//FIXME: loading these seems seriously derpy. unify all.
	QuaZip zip(PathCombine(m_instance->minecraftRoot(), "bin", "modpack.jar") );
	if(!zip.open(QuaZip::mdUnzip))
	{
		return;
	}
	zip.setCurrentFile("version.json");
	QuaZipFile inFile(&zip);
	if(!inFile.open(QIODevice::ReadOnly) || inFile.getZipError()!=UNZ_OK)
	{
		return;
	}

	QJsonDocument doc = QJsonDocument::fromJson(inFile.readAll());
	inFile.close();
	zip.close();
	if (doc.isNull())
	{
		throw VersionIncomplete("net.technicpack");
	}
	auto vfile = VersionFile::fromJson(doc, "version.json", false);

	auto vpatch = std::dynamic_pointer_cast<ProfilePatch>(vfile);
	if (!vpatch)
	{
		throw VersionIncomplete("net.technicpack");
	}
	vpatch->setOrder(0);
	vfile->setVanilla(true);

	// FIXME: CRASHY!
	auto vanillaThing = std::dynamic_pointer_cast<MinecraftVersion>(profile->versionPatch("net.minecraft"));
	if(!vanillaThing)
	{
		qWarning() << "Can't use the vanilla patch thing. Oh my.";
		return;
	}

	profile->appendPatch(processForgeJsonForTechnic(vfile,vanillaThing));
}

void SolderProfileStrategy::load()
{
	profile->clearPatches();

	loadDefaultBuiltinPatches();
	loadModpackJarPatch();

	profile->finalize();
}

bool SolderProfileStrategy::removePatch(ProfilePatchPtr patch)
{
	return false;
}

bool SolderProfileStrategy::resetOrder()
{
	return false;
}

bool SolderProfileStrategy::saveOrder(ProfileUtils::PatchOrder order)
{
	return false;
}
