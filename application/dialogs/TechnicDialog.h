#pragma once
#include "QMLDialog.h"
#include "technic/PackModel.h"

class TechnicDialog : public QMLDialog
{
	Q_OBJECT
public:
	explicit TechnicDialog(QWidget * parent = 0);

public:
	Q_INVOKABLE void setCurrent(int i);
	SolderPackInfoPtr getPack();
	QString getInstanceName();
	SolderVersionPtr getVersion();

private:
	int m_currentPack = -1;
	PackModel * m_model;
	VersionModel * m_versionModel;
};
