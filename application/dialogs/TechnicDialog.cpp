#include "TechnicDialog.h"
#include <QQmlContext>
#include <QQuickView>
#include "technic/PackModel.h"
#include "technic/ImageProvider.h"

TechnicDialog::TechnicDialog(QWidget *parent)
	: QMLDialog(parent)
{
	m_model = new PackModel(this);
	m_versionModel = new VersionModel(nullptr, this);
	setWindowTitle(tr("Install Technic Pack"));
	setWindowIcon(QIcon::fromTheme("technic"));
	QQmlContext *ctxt = m_view->rootContext();
	ctxt->setContextProperty("packModel", m_model);
	ctxt->setContextProperty("versionModel", m_versionModel);
	m_view->engine()->addImageProvider(QLatin1String("url"), new ImageProvider());
	setUrl(QUrl(QStringLiteral("qrc:///qml/technic/technic.qml")));
}

void TechnicDialog::setCurrent(int i)
{
	if(m_currentPack == i)
	{
		return;
	}
	m_currentPack = -1;
	auto pack = m_model->packByIndex(i);
	if(pack)
	{
		m_versionModel->reset(pack);
	}
	else
	{
		m_versionModel->reset(nullptr);
	}
}


SolderPackInfoPtr TechnicDialog::getPack()
{
	auto index = getPropertyValue("currentPack", -1).toInt();
	return m_model->packByIndex(index);
}

SolderVersionPtr TechnicDialog::getVersion()
{
	auto versionId = getPropertyValue("currentVersion", QString("invalid")).toString();
	return m_versionModel->versionById(versionId);
}

QString TechnicDialog::getInstanceName()
{
	auto pack = getPack();
	if(!pack)
	{
		return "invalid";
	}
	auto name = getPropertyValue("instanceName", pack->display_name).toString();
	return name;
}
