#include "minecraft/OneSixProfileStrategy.h"

class SolderInstance;

class SolderProfileStrategy: public OneSixProfileStrategy
{
public:
	SolderProfileStrategy(SolderInstance* instance);
	virtual ~SolderProfileStrategy() {};

public:
	virtual bool installJarMods(QStringList filepaths) override;
	virtual void load() override;
	virtual bool removePatch(ProfilePatchPtr patch) override;
	virtual bool resetOrder() override;
	virtual bool saveOrder(ProfileUtils::PatchOrder order) override;

protected:
	void loadModpackJarPatch();
};
