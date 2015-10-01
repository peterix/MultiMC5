#include "tasks/Task.h"
#include <memory>
#include <QObjectPtr.h>

class SolderInstance;
class NetJob;
class NetAction;
struct SolderModEntry;

typedef std::shared_ptr<SolderModEntry> SolderMoEntryPtr;

class SolderUpdate : public Task
{
	Q_OBJECT
public:
	explicit SolderUpdate(SolderInstance *inst, QObject *parent = 0)
		: Task(parent), m_inst(inst){};
	virtual void executeTask();

private slots:
	void packVersionStart();
	void packVersionFinished();
	void packVersionFailed();
	void versionProgress(int, qint64, qint64);

	void packStart();
	void packFinished();
	void packFailed();
	void packProgress(qint64,qint64);

private:
	std::shared_ptr<NetAction> packVersionDownload;
	QObjectPtr<NetJob> packDownload;

	SolderInstance *m_inst;
	QList <std::shared_ptr<SolderModEntry>> m_mods;
};

