#pragma once

#include "minecraft/OneSixInstance.h"
#include "SolderVersion.h"
class Task;

class SolderInstance : public OneSixInstance
{
	Q_OBJECT
public:
	explicit SolderInstance(SettingsObjectPtr globalSettings, SettingsObjectPtr settings, const QString &rootDir);
    virtual ~SolderInstance(){};

	virtual void createProfile() override;
	void setSolderVersion(SolderVersionPtr url);
	SolderVersionPtr solderVersion();
	virtual QList<Mod> getJarMods() const override;

	virtual std::shared_ptr< Task > createUpdateTask() override;
private:
	SolderVersionPtr m_solderVersion;
};
